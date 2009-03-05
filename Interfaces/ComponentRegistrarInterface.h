// For conditions of distribution and use, see copyright notice in license.txt

#ifndef incl_Interfaces_ComponentRegistrarInterface_h
#define incl_Interfaces_ComponentRegistrarInterface_h

namespace Foundation
{
    class Framework;

    class ComponentRegistrarInterface
    {
    public:
        ComponentRegistrarInterface()  {}
        virtual ~ComponentRegistrarInterface() {}
        
        virtual void _register(Framework *framework) = 0;

        virtual void _unregister(Framework *framework) = 0;
    };

    typedef boost::shared_ptr<ComponentRegistrarInterface> ComponentRegistrarInterfacePtr;
}

#endif
