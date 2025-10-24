#include <iostream>
#include "type_traits.h"
#include  "storage.h"
#include <assert.h>
// TIP To <b>Run</b> code, press <shortcut actionId="Run"/> or click the <icon src="AllIcons.Actions.Execute"/> icon in the gutter.

inline bool destroyedA = false;
inline bool destroyedB = false;
inline bool destroyedC = false;

struct A {
    ~A() { destroyedA = true; }
};
struct B {
    ~B() { destroyedB = true; }
};
struct C {
    ~C() { destroyedC = true; }
};

alignas(A) unsigned char storageA[sizeof(A)];
alignas(B) unsigned char storageB[sizeof(B)];
alignas(C) unsigned char storageC[sizeof(C)];

int main()
{
    using dispatcher = destructor_dispatcher<A, B, C>;

    // Test index 0 => doit détruire un A
    {
        destroyedA = destroyedB = destroyedC = false;
        new (storageA) A();
        dispatcher::destroy(0, storageA);

        assert(destroyedA == true);
        assert(destroyedB == false);
        assert(destroyedC == false);
    }


    return 0;
    // TIP See CLion help at <a href="https://www.jetbrains.com/help/clion/">jetbrains.com/help/clion/</a>. Also, you can try interactive lessons for CLion by selecting 'Help | Learn IDE Features' from the main menu.
}