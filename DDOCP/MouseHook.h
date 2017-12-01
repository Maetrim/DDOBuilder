// MouseHook.h
//
#pragma once
#include <vector>

// the mouse hook class provides application functionality beyond the standard
// it will notify windows when the mouse enters or leaves specific rectangles
// in screen coordinates. This allows better tooltip show/hide functionality
struct AreaOfInterest
{
    UINT m_areaHandle;          // returned to caller, use this to update rectangle on a window re-size/move
    CRect m_rectangle;          // rectangle in screen coordinates
    HWND m_whoToNotify;         // window to send message to
    UINT m_enterNotification;   // message to send on an enter rectangle
    UINT m_exitNotification;    // message to send on an exit rectangle
    bool m_bIn;                 // true if mouse was in this rectangle last time
};

#define WM_MOUSEENTER       WM_USER + 999

class MouseHook
{
    public:
        MouseHook();            // hooks in constructor
        ~MouseHook();           // unhooks in destructor

        UINT AddRectangleToMonitor(
                HWND hNotify,
                const CRect & rect,
                UINT nIDEnterNotification,
                UINT nExitNotification,
                bool bStartsInside);
        void DeleteRectangleToMonitor(UINT handle);
        bool UpdateRectangle(UINT handle, const CRect & rect);
        void SaveState();
        void RestoreState();

    private:
        static LRESULT CALLBACK MouseProc(_In_ int nCode, _In_ WPARAM wParam, _In_ LPARAM lParam);
        void ProcessMessage(int nCode, WPARAM wParam, LPARAM lParam);
        BOOL IsUnderMouse(const CPoint & mouse, HWND hwnd) const;
        static MouseHook * theHook;     // so static function can find the object
        HHOOK m_hookHandle;
        std::vector<AreaOfInterest> m_areasOfInterest;
        size_t m_nextHandle;
        std::vector<AreaOfInterest> m_savedState;
};

