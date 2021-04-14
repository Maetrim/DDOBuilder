// ArtifactFiligree.h
//
#pragma once
#include "Filigree.h"

class ArtifactFiligree :
    public Filigree
{
    public:
        ArtifactFiligree(void);
        void Write(XmlLib::SaxWriter* writer) const;

        bool operator==(const ArtifactFiligree & other) const;

    protected:
        XmlLib::SaxContentElementInterface* StartElement(
            const XmlLib::SaxString& name,
            const XmlLib::SaxAttributes& attributes);

        virtual void EndElement();

        #define ArtifactFiligree_PROPERTIES(_)

        DL_DECLARE_ACCESS(ArtifactFiligree_PROPERTIES)
        DL_DECLARE_VARIABLES(ArtifactFiligree_PROPERTIES)
};
