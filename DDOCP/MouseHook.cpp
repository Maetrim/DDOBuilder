// MouseHook.cpp
//
#include "StdAfx.h"
#include "MouseHook.h"

MouseHook * MouseHook::theHook = NULL;

MouseHook::MouseHook() :
    m_nextHandle(0),
    m_bDisableReporting(false)
{
    // we install the hook in the constructor
    m_hookHandle = SetWindowsHookEx(
            WH_MOUSE,
            MouseHook::MouseProc,
            AfxGetInstanceHandle(),
            GetCurrentThreadId());
    ASSERT(theHook == NULL);
    theHook = this;     // so callback function can find this object
}

MouseHook::~MouseHook()
{
    // uninstall the hook in the destructor
    if (m_hookHandle != NULL)
    {
        UnhookWindowsHookEx(m_hookHandle);
    }
    theHook = NULL;
}

LRESULT CALLBACK MouseHook::MouseProc(_In_ int nCode, _In_ WPARAM wParam, _In_ LPARAM lParam)
{
    // pass on to the member function
    theHook->ProcessMessage(nCode, wParam, lParam);
    // and finally call the next hook in the chain
    return CallNextHookEx(theHook->m_hookHandle, nCode, wParam, lParam);
}

void MouseHook::ProcessMessage(int nCode, WPARAM wParam, LPARAM lParam)
{
    // can be disabled when a dialog is displayed
    if (!m_bDisableReporting)
    {
        // were only interested in mouse move messages as we notify when the mouse
        // enters or leaves the specified rectangles.
        if (wParam == WM_MOUSEMOVE)
        {
            // look through each item and see if its in/out state has changed. If it has
            // post a message to the relevant window
            MOUSEHOOKSTRUCT * pMHS = (MOUSEHOOKSTRUCT*)(lParam);
            CPoint mouse = pMHS->pt;        // mouse location in screen coordinates
            // notify all leave areas before any enter areas
            // this stops an enter notification being cancelled for a leave from
            // a different area.
            for (size_t i = 0; i < m_areasOfInterest.size(); ++i)
            {
                if (!m_areasOfInterest[i].m_rectangle.PtInRect(mouse))
                {
                    // was it in the rectangle before? if so, notify
                    if (m_areasOfInterest[i].m_bIn)
                    {
                        // no longer inside, notify
                        m_areasOfInterest[i].m_bIn = false;
                        // post message as we do not want to wait for it to be processed right now
                        PostMessage(
                                m_areasOfInterest[i].m_whoToNotify,
                                m_areasOfInterest[i].m_exitNotification,
                                m_areasOfInterest[i].m_areaHandle,
                                0L);
                    }
                }
            }
            for (size_t i = 0; i < m_areasOfInterest.size(); ++i)
            {
                if (m_areasOfInterest[i].m_rectangle.PtInRect(mouse))
                {
                    // is it now in the rectangle, notify if it was not previously inside
                    if (!m_areasOfInterest[i].m_bIn)
                    {
                        // now inside, notify
                        m_areasOfInterest[i].m_bIn = true;
                        // post message as we do not want to wait for it to be processed right now
                        PostMessage(
                                m_areasOfInterest[i].m_whoToNotify,
                                m_areasOfInterest[i].m_enterNotification,
                                m_areasOfInterest[i].m_areaHandle,
                                0L);
                    }
                }
            }
        }
    }
}

UINT MouseHook::AddRectangleToMonitor(
        HWND hNotify,
        const CRect & rect,
        UINT nIDEnterNotification,
        UINT nExitNotification,
        bool bStartsInside)
{
    // return value is the handle to this object about to be created
    AreaOfInterest aoi;
    aoi.m_areaHandle = m_nextHandle++;  // increment after assigned
    aoi.m_rectangle = rect;
    aoi.m_whoToNotify = hNotify;
    aoi.m_enterNotification = nIDEnterNotification;
    aoi.m_exitNotification = nExitNotification;
    aoi.m_bIn = bStartsInside;
    m_areasOfInterest.push_back(aoi);   // add to the vector
    return aoi.m_areaHandle;
}

void MouseHook::DeleteRectangleToMonitor(UINT handle)
{
    // look for and remove the rectangle from the vector
    bool found = false;
    for (size_t i = 0; i < m_areasOfInterest.size(); ++i)
    {
        if (m_areasOfInterest[i].m_areaHandle == handle)
        {
            m_areasOfInterest.erase(m_areasOfInterest.begin() + i);
            found = true;
            break;
        }
    }
    ASSERT(found);
}

bool MouseHook::UpdateRectangle(UINT handle, const CRect & rect)
{
    bool found = false;
    // update the rectangle for this area of interest
    // this happens in the window with the areas of interest is moved or resized.
    for (size_t i = 0; i < m_areasOfInterest.size(); ++i)
    {
        if (m_areasOfInterest[i].m_areaHandle == handle)
        {
            m_areasOfInterest[i].m_rectangle = rect;
            found = true;
            break;
        }
    }
    return found;
}

void MouseHook::SetDisabledState(bool state)
{
    // reporting of enter / leave messages needs to be disabled when a dialog
    // is being displayed as we do not want tooltips being shown / hidden for
    // controls behind the active dialog.
    m_bDisableReporting = state;
}

