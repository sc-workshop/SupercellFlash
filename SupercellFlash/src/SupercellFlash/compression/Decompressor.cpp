#include "SupercellFlash/compression/Decompressor.h"

#include "SupercellFlash/error/UnknownFileMagicException.h"
#include "SupercellFlash/error/UnknownFileVersionException.h"

#include "SupercellFlash/compression/backend/LzmaCompression.h"
#include "SupercellFlash/compression/backend/ZstdCompression.h"

#include <iostream>

namespace sc
{
    void Decompressor::decompress(ByteStream& inStream, ByteStream& outStream)
    {
        uint16_t magic = inStream.readUnsignedShort();
        std::cout << magic << std::endl;
        if (magic != 0x5343)
            throw UnknownFileMagicException("Unknown file magic: " + magic);

        int32_t version = inStream.readInt();
        if (version == 4)
            version = inStream.readInt();

        int32_t hashLength = inStream.readInt();
        inStream.skip(hashLength);

        // TODO: Lzham
        switch (version)
        {
        case 1:
            LZMA::decompress(inStream, outStream);
            break;

        case 2:
        case 3:
            ZSTD::decompress(inStream, outStream);
            break;

        default:
            throw UnknownFileVersionException("Unknown file version: " + version);
        }
    }
}
