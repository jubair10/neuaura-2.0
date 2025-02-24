#include <Python.h>

int main()
{
    Py_Initialize();
    PyObject * platform_module = PyImport_ImportModule("platform");
    if(!platform_module) {
        printf("\nCould not import platform module.\n");
        return 0;
    }

    PyObject* system_function = PyObject_GetAttrString(platform_module, "system");
    if(system_function && PyCallable_Check(system_function)) {
        PyObject * system_obj = PyObject_CallObject(system_function, NULL);
        
        PyObject* str = PyUnicode_AsEncodedString(system_obj, "utf-8", "~E~");
        const char * system_str = PyBytes_AS_STRING(str);
        printf("\n\nPlatform name is %s \n\n", system_str);

        Py_XDECREF(system_function);
        Py_XDECREF(system_obj);
        Py_XDECREF(str);

        printf("\nCould not call platform.system() function.\n");
    }

    return 0;
}