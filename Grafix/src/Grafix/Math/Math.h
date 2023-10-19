#pragma once

namespace Grafix
{
    namespace Math
    {
        int NChooseK(int n, int k)
        {
            if (n < k)
                return 0;

            if (k == 0 || k == n)
                return 1;

            return NChooseK(n - 1, k - 1) + NChooseK(n - 1, k);
        }
    }
}
