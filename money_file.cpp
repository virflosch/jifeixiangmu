#define _CRT_SECURE_NO_WARNINGS

#include "money_file.h"
#include "global.h"
#include <fstream>

int saveMoney(const Money *pMoney, const char *pPath)
{
    const char *path = (pPath != nullptr) ? pPath : MONEY_FILE_PATH;
    std::ofstream file(path, std::ios::binary | std::ios::app);
    if (!file)
    {
        return -1;
    }

    file.write(reinterpret_cast<const char *>(pMoney), sizeof(Money));
    if (!file)
    {
        return -2;
    }

    file.close();
    return 0;
}