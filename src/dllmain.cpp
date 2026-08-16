#include "Entry.hpp"

BOOL APIENTRY DllMain(HMODULE module, DWORD reason, LPVOID reserved) {
    switch (reason) {
        case DLL_PROCESS_ATTACH:
            if (!Entry::OnAttach(module)) {
                return FALSE;
            }
            break;

        case DLL_PROCESS_DETACH:
            Entry::OnDetach(reserved != nullptr);
            break;
    }

    return TRUE;
}
