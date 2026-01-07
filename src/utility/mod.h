#pragma once
#include <d3d9.h>

#include <cstdint>
#include <cstring>

#include <map>
#include <filesystem>
#include <vector>
#include <string>
#include <algorithm>
#include <cctype>
#include <chrono>
#include <new>

// =============================================================================
// Resource Type Enum
// =============================================================================

#ifndef TLRESOURCE_TYPE_DEFINED
#define TLRESOURCE_TYPE_DEFINED

#endif

// =============================================================================
// GIF Animation Support
// =============================================================================

struct GifFrame {
    std::vector<uint8_t> textureData;
    uint16_t delayMs;
};

struct AnimatedMod {
    std::vector<GifFrame> frames;
    uint16_t totalDurationMs;
    size_t currentFrame;
    std::chrono::steady_clock::time_point lastFrameTime;
    bool isPlaying;
    bool loop;
    
    AnimatedMod() : totalDurationMs(0), currentFrame(0), isPlaying(true), loop(true) {
        lastFrameTime = std::chrono::steady_clock::now();
    }
    
    [[nodiscard]] bool isAnimated() const { return frames.size() > 1; }
    [[nodiscard]] size_t frameCount() const { return frames.empty() ? 0 : frames.size(); }
};

// =============================================================================
// Mod Structure
// =============================================================================

struct Mod {
    std::filesystem::path Path;
    int Type;
    std::vector<uint8_t> Data;
    AnimatedMod animation;
    
    [[nodiscard]] bool isAnimated() const { return Type == 4 && animation.isAnimated(); }
};

struct modGenericMesh {
    Mod* mod;
    std::vector<float> vertices;
    std::vector<uint16_t> indices;
    IDirect3DVertexBuffer9* vertexBuffer = nullptr;
    IDirect3DIndexBuffer9* indexBuffer = nullptr;
    UINT stride = 16;
    UINT numVertices = 0;
    UINT numIndices = 0;
};

extern std::map<uint32_t, Mod> Mods;
extern Mod* dbgReplaceMesh;

// =============================================================================
// String Utilities
// =============================================================================

inline std::string transformToLower(const std::string& name) {
    std::string res = name;
    std::transform(res.begin(), res.end(), res.begin(), 
                   [](unsigned char c) { return std::tolower(c); });
    return res;
}

// =============================================================================
// Mod Lookup Functions
// =============================================================================

inline bool hasMod(uint32_t hash) {
    return Mods.find(hash) != Mods.end();
}

inline Mod* getMod(uint32_t hash, int type = -1) {
    if (type == -1) {
        auto it = Mods.find(hash);
        if (it != Mods.end())
            return &it->second;
    } else {
        for (auto& [ihash, mod] : Mods)
            if (hash == ihash && mod.Type == type)
                return &mod;
    }
    return nullptr;
}

inline uint8_t* getModDataByHash(uint32_t hash) {
    if (hasMod(hash)) {
        if (auto mod = getMod(hash)) {
            if (mod->isAnimated()) {
                auto& anim = mod->animation;
                if (anim.currentFrame < anim.frames.size()) {
                    auto& frame = anim.frames[anim.currentFrame];
                    if (!frame.textureData.empty())
                        return frame.textureData.data();
                }
            }
            if (!mod->Data.empty())
                return mod->Data.data();
        }
    }
    return nullptr;
}

inline Mod* getModByFilemame(const std::string& name) {
    std::string search = transformToLower(name);
    for (auto& [hash, mod] : Mods) {
        std::string filename = transformToLower(mod.Path.filename().string());
        if (filename == search)
            return &mod;
    }
    return nullptr;
}

// =============================================================================
// GIF Parser - Internal Structures
// =============================================================================

namespace gif {

#pragma pack(push, 1)
struct Header {
    char signature[3];
    char version[3];
};

struct LogicalScreenDescriptor {
    uint16_t width;
    uint16_t height;
    uint8_t  packed;
    uint8_t  bgColorIndex;
    uint8_t  pixelAspectRatio;
};

struct ImageDescriptor {
    uint16_t left;
    uint16_t top;
    uint16_t width;
    uint16_t height;
    uint8_t  packed;
};

struct GraphicsControlExtension {
    uint8_t  packed;
    uint16_t delayTime;
    uint8_t  transparentIdx;
};
#pragma pack(pop)

class LzwDecoder {
public:
    static constexpr int MAX_CODE_SIZE = 12;
    static constexpr int MAX_CODES = 1 << MAX_CODE_SIZE;
    
    struct Entry {
        int16_t prefix;
        uint8_t suffix;
    };
    
    int minCodeSize;
    int codeSize;
    int clearCode;
    int endCode;
    int nextCode;
    Entry table[MAX_CODES];
    
    const uint8_t* data;
    size_t dataSize;
    size_t bytePos;
    int bitPos;
    
    void init(int minSize, const uint8_t* src, size_t srcSize) {
        minCodeSize = minSize;
        codeSize = minSize + 1;
        clearCode = 1 << minSize;
        endCode = clearCode + 1;
        nextCode = endCode + 1;
        
        data = src;
        dataSize = srcSize;
        bytePos = 0;
        bitPos = 0;
        
        for (int i = 0; i < clearCode; ++i) {
            table[i].prefix = -1;
            table[i].suffix = static_cast<uint8_t>(i);
        }
    }
    
    int readCode() {
        if (bytePos >= dataSize) return endCode;
        
        int code = 0;
        int bitsRead = 0;
        
        while (bitsRead < codeSize) {
            if (bytePos >= dataSize) break;
            
            int bitsAvailable = 8 - bitPos;
            int bitsToRead = (std::min)(codeSize - bitsRead, bitsAvailable);
            int mask = (1 << bitsToRead) - 1;
            
            code |= ((data[bytePos] >> bitPos) & mask) << bitsRead;
            bitsRead += bitsToRead;
            bitPos += bitsToRead;
            
            if (bitPos >= 8) {
                bitPos = 0;
                bytePos++;
            }
        }
        
        return code;
    }
    
    void outputCode(int code, std::vector<uint8_t>& output) {
        if (code < 0 || code >= MAX_CODES) return;
        
        std::vector<uint8_t> temp;
        int c = code;
        while (c >= 0 && temp.size() < 4096) {
            temp.push_back(table[c].suffix);
            c = table[c].prefix;
        }
        
        for (auto it = temp.rbegin(); it != temp.rend(); ++it)
            output.push_back(*it);
    }
    
    uint8_t firstChar(int code) {
        while (code >= 0 && table[code].prefix >= 0)
            code = table[code].prefix;
        return (code >= 0) ? table[code].suffix : 0;
    }
    
    void addEntry(int prefix, uint8_t suffix) {
        if (nextCode < MAX_CODES) {
            table[nextCode].prefix = static_cast<int16_t>(prefix);
            table[nextCode].suffix = suffix;
            nextCode++;
            
            if (nextCode > (1 << codeSize) && codeSize < MAX_CODE_SIZE)
                codeSize++;
        }
    }
    
    void reset() {
        codeSize = minCodeSize + 1;
        nextCode = endCode + 1;
    }
};

inline size_t collectSubBlocks(const uint8_t* data, size_t offset, size_t maxSize, 
                               std::vector<uint8_t>& output) {
    size_t pos = offset;
    output.clear();
    
    while (pos < maxSize) {
        uint8_t blockSize = data[pos++];
        if (blockSize == 0) break;
        if (pos + blockSize > maxSize) break;
        
        output.insert(output.end(), data + pos, data + pos + blockSize);
        pos += blockSize;
    }
    
    return pos - offset;
}

} // namespace gif

// =============================================================================
// Convert RGBA to DDS
// =============================================================================

inline std::vector<uint8_t> convertRGBAToDDS(const std::vector<uint8_t>& rgba, 
                                              uint16_t width, uint16_t height) {
    constexpr uint32_t DDS_MAGIC = 0x20534444;
    
    struct DdsHeader {
        uint32_t magic;
        uint32_t size;
        uint32_t flags;
        uint32_t height;
        uint32_t width;
        uint32_t pitchOrLinearSize;
        uint32_t depth;
        uint32_t mipMapCount;
        uint32_t reserved1[11];
        uint32_t pfSize;
        uint32_t pfFlags;
        uint32_t pfFourCC;
        uint32_t pfRGBBitCount;
        uint32_t pfRBitMask;
        uint32_t pfGBitMask;
        uint32_t pfBBitMask;
        uint32_t pfABitMask;
        uint32_t caps1;
        uint32_t caps2;
        uint32_t reservedCaps[2];
        uint32_t reserved2;
    };
    
    DdsHeader header = {};
    header.magic = DDS_MAGIC;
    header.size = 124;
    header.flags = 0x0002100F;
    header.height = height;
    header.width = width;
    header.pitchOrLinearSize = width * 4;
    header.depth = 1;
    header.mipMapCount = 1;
    header.pfSize = 32;
    header.pfFlags = 0x41;
    header.pfRGBBitCount = 32;
    header.pfRBitMask = 0x00FF0000;
    header.pfGBitMask = 0x0000FF00;
    header.pfBBitMask = 0x000000FF;
    header.pfABitMask = 0xFF000000;
    header.caps1 = 0x1000;
    
    std::vector<uint8_t> result;
    result.resize(sizeof(DdsHeader) + rgba.size());
    
    std::memcpy(result.data(), &header, sizeof(DdsHeader));
    
    uint8_t* dst = result.data() + sizeof(DdsHeader);
    for (size_t i = 0; i < rgba.size(); i += 4) {
        dst[i + 0] = rgba[i + 2];
        dst[i + 1] = rgba[i + 1];
        dst[i + 2] = rgba[i + 0];
        dst[i + 3] = rgba[i + 3];
    }
    
    return result;
}

// =============================================================================
// GIF Parser Implementation
// =============================================================================

inline bool parseGifToMod(const std::vector<uint8_t>& gifData, Mod& outMod) {
    constexpr size_t MAX_GIF_DIMENSION = 2048;
    constexpr size_t MAX_FRAME_COUNT = 256;
    constexpr size_t MAX_CANVAS_BYTES = MAX_GIF_DIMENSION * MAX_GIF_DIMENSION * 4;
    
    if (gifData.size() < sizeof(gif::Header) + sizeof(gif::LogicalScreenDescriptor)) {
        printf("GIF Error: File too small\n");
        return false;
    }
    
    const uint8_t* data = gifData.data();
    size_t pos = 0;
    
    auto* header = reinterpret_cast<const gif::Header*>(data);
    if (std::memcmp(header->signature, "GIF", 3) != 0) {
        printf("GIF Error: Invalid signature\n");
        return false;
    }
    pos += sizeof(gif::Header);
    
    auto* lsd = reinterpret_cast<const gif::LogicalScreenDescriptor*>(data + pos);
    uint16_t canvasWidth = lsd->width;
    uint16_t canvasHeight = lsd->height;
    
    if (canvasWidth == 0 || canvasHeight == 0 || 
        canvasWidth > MAX_GIF_DIMENSION || canvasHeight > MAX_GIF_DIMENSION) {
        printf("GIF Error: Invalid dimensions %dx%d\n", canvasWidth, canvasHeight);
        return false;
    }
    
    bool hasGlobalColorTable = (lsd->packed & 0x80) != 0;
    int globalColorTableSize = hasGlobalColorTable ? (1 << ((lsd->packed & 0x07) + 1)) : 0;
    pos += sizeof(gif::LogicalScreenDescriptor);
    
    std::vector<uint8_t> globalColorTable;
    if (hasGlobalColorTable) {
        size_t tableBytes = globalColorTableSize * 3;
        if (pos + tableBytes > gifData.size()) {
            printf("GIF Error: Truncated color table\n");
            return false;
        }
        globalColorTable.assign(data + pos, data + pos + tableBytes);
        pos += tableBytes;
    }
    
    std::vector<uint8_t> canvas;
    std::vector<uint8_t> previousCanvas;
    try {
        size_t canvasBytes = static_cast<size_t>(canvasWidth) * canvasHeight * 4;
        if (canvasBytes > MAX_CANVAS_BYTES) {
            printf("GIF Error: Canvas too large\n");
            return false;
        }
        canvas.resize(canvasBytes, 0);
    } catch (const std::bad_alloc&) {
        printf("GIF Error: Failed to allocate canvas\n");
        return false;
    }
    
    gif::GraphicsControlExtension gce = {};
    gce.delayTime = 10;
    bool hasGce = false;
    
    outMod.animation = AnimatedMod{};
    outMod.animation.totalDurationMs = 0;
    
    printf("GIF: Parsing %dx%d...\n", canvasWidth, canvasHeight);
    
    while (pos < gifData.size() && outMod.animation.frames.size() < MAX_FRAME_COUNT) {
        uint8_t blockType = data[pos++];
        
        if (blockType == 0x3B) break;
        
        if (blockType == 0x21) {
            if (pos >= gifData.size()) break;
            uint8_t extLabel = data[pos++];
            
            if (extLabel == 0xF9) {
                if (pos >= gifData.size()) break;
                uint8_t blockSize = data[pos++];
                if (blockSize >= 4 && pos + 4 <= gifData.size()) {
                    gce.packed = data[pos];
                    gce.delayTime = *reinterpret_cast<const uint16_t*>(data + pos + 1);
                    gce.transparentIdx = data[pos + 3];
                    hasGce = true;
                }
                pos += blockSize;
                if (pos < gifData.size()) pos++;
            } else {
                while (pos < gifData.size()) {
                    uint8_t subBlockSize = data[pos++];
                    if (subBlockSize == 0) break;
                    pos += subBlockSize;
                }
            }
        }
        else if (blockType == 0x2C) {
            if (pos + sizeof(gif::ImageDescriptor) > gifData.size()) break;
            
            auto* imgDesc = reinterpret_cast<const gif::ImageDescriptor*>(data + pos);
            pos += sizeof(gif::ImageDescriptor);
            
            int disposalMethod = hasGce ? ((gce.packed >> 2) & 0x07) : 0;
            bool hasTransparency = hasGce && (gce.packed & 0x01);
            uint8_t transparentIdx = hasTransparency ? gce.transparentIdx : 0xFF;
            
            std::vector<uint8_t>* colorTable = &globalColorTable;
            std::vector<uint8_t> localColorTable;
            
            if (imgDesc->packed & 0x80) {
                int localSize = 1 << ((imgDesc->packed & 0x07) + 1);
                size_t tableBytes = localSize * 3;
                if (pos + tableBytes > gifData.size()) break;
                localColorTable.assign(data + pos, data + pos + tableBytes);
                colorTable = &localColorTable;
                pos += tableBytes;
            }
            
            if (pos >= gifData.size()) break;
            int lzwMinSize = data[pos++];
            
            std::vector<uint8_t> compressedData;
            pos += gif::collectSubBlocks(data, pos, gifData.size(), compressedData);
            
            std::vector<uint8_t> indexData;
            indexData.reserve(imgDesc->width * imgDesc->height);
            
            gif::LzwDecoder decoder;
            decoder.init(lzwMinSize, compressedData.data(), compressedData.size());
            
            int prevCode = -1;
            while (true) {
                int code = decoder.readCode();
                if (code == decoder.endCode) break;
                
                if (code == decoder.clearCode) {
                    decoder.reset();
                    prevCode = -1;
                    continue;
                }
                
                if (prevCode < 0) {
                    decoder.outputCode(code, indexData);
                    prevCode = code;
                    continue;
                }
                
                if (code < decoder.nextCode) {
                    decoder.outputCode(code, indexData);
                    decoder.addEntry(prevCode, decoder.firstChar(code));
                } else {
                    uint8_t c = decoder.firstChar(prevCode);
                    decoder.addEntry(prevCode, c);
                    decoder.outputCode(decoder.nextCode - 1, indexData);
                }
                prevCode = code;
            }
            
            if (disposalMethod == 3)
                previousCanvas = canvas;
            
            bool interlaced = (imgDesc->packed & 0x40) != 0;
            size_t srcIdx = 0;
            
            auto renderRow = [&](int y) {
                if (y >= imgDesc->height) return;
                int canvasY = imgDesc->top + y;
                if (canvasY >= canvasHeight) return;
                
                for (int x = 0; x < imgDesc->width && srcIdx < indexData.size(); ++x, ++srcIdx) {
                    int canvasX = imgDesc->left + x;
                    if (canvasX >= canvasWidth) continue;
                    
                    uint8_t idx = indexData[srcIdx];
                    if (hasTransparency && idx == transparentIdx) continue;
                    
                    size_t colorOffset = idx * 3;
                    if (colorOffset + 2 >= colorTable->size()) continue;
                    
                    size_t pixelOffset = (canvasY * canvasWidth + canvasX) * 4;
                    canvas[pixelOffset + 0] = (*colorTable)[colorOffset + 0];
                    canvas[pixelOffset + 1] = (*colorTable)[colorOffset + 1];
                    canvas[pixelOffset + 2] = (*colorTable)[colorOffset + 2];
                    canvas[pixelOffset + 3] = 255;
                }
            };
            
            if (interlaced) {
                const int passes[4][2] = {{0, 8}, {4, 8}, {2, 4}, {1, 2}};
                for (auto& pass : passes)
                    for (int y = pass[0]; y < imgDesc->height; y += pass[1])
                        renderRow(y);
            } else {
                for (int y = 0; y < imgDesc->height; ++y)
                    renderRow(y);
            }
            
            try {
                GifFrame frame;
                frame.delayMs = gce.delayTime * 10;
                if (frame.delayMs == 0) frame.delayMs = 100;
                frame.textureData = convertRGBAToDDS(canvas, canvasWidth, canvasHeight);
                
                if (!frame.textureData.empty()) {
                    outMod.animation.frames.push_back(std::move(frame));
                    outMod.animation.totalDurationMs += outMod.animation.frames.back().delayMs;
                }
            } catch (const std::bad_alloc&) {
                printf("GIF Error: Out of memory at frame %zu\n", outMod.animation.frames.size());
                break;
            }
            
            switch (disposalMethod) {
                case 2:
                    for (int y = imgDesc->top; y < imgDesc->top + imgDesc->height && y < canvasHeight; ++y) {
                        for (int x = imgDesc->left; x < imgDesc->left + imgDesc->width && x < canvasWidth; ++x) {
                            size_t offset = (y * canvasWidth + x) * 4;
                            canvas[offset + 0] = canvas[offset + 1] = canvas[offset + 2] = canvas[offset + 3] = 0;
                        }
                    }
                    break;
                case 3:
                    canvas = previousCanvas;
                    break;
            }
            
            hasGce = false;
        }
        else break;
    }
    
    if (outMod.animation.frames.empty())
        return false;
    
    outMod.Data = outMod.animation.frames[0].textureData;
    outMod.Type = 4;
    
    printf("GIF loaded: %zu frames, %ums duration\n", 
           outMod.animation.frames.size(), outMod.animation.totalDurationMs);
    
    return true;
}

// =============================================================================
// Animation Update - Call once per frame in render loop
// =============================================================================

inline void updateAnimatedMods() {
    auto now = std::chrono::steady_clock::now();
    
    for (auto& [hash, mod] : Mods) {
        if (!mod.isAnimated()) continue;
        if (!mod.animation.isPlaying) continue;
        
        auto& anim = mod.animation;
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
            now - anim.lastFrameTime
        ).count();
        
        if (anim.currentFrame >= anim.frames.size()) 
            anim.currentFrame = 0;
        
        const auto& currentFrame = anim.frames[anim.currentFrame];
        
        if (elapsed >= currentFrame.delayMs) {
            anim.currentFrame++;
            
            if (anim.currentFrame >= anim.frames.size()) {
                if (anim.loop)
                    anim.currentFrame = 0;
                else {
                    anim.currentFrame = anim.frames.size() - 1;
                    anim.isPlaying = false;
                }
            }
            
            anim.lastFrameTime = now;
        }
    }
}

// =============================================================================
// Animation Control Functions
// =============================================================================

inline void setModAnimationPlaying(uint32_t hash, bool playing) {
    if (auto* mod = getMod(hash)) {
        mod->animation.isPlaying = playing;
        if (playing)
            mod->animation.lastFrameTime = std::chrono::steady_clock::now();
    }
}

inline void setModAnimationLoop(uint32_t hash, bool loop) {
    if (auto* mod = getMod(hash))
        mod->animation.loop = loop;
}

inline void resetModAnimation(uint32_t hash) {
    if (auto* mod = getMod(hash)) {
        mod->animation.currentFrame = 0;
        mod->animation.lastFrameTime = std::chrono::steady_clock::now();
        mod->animation.isPlaying = true;
    }
}

inline void setModAnimationFrame(uint32_t hash, size_t frameIndex) {
    if (auto* mod = getMod(hash)) {
        if (!mod->animation.frames.empty())
            mod->animation.currentFrame = (std::min)(frameIndex, mod->animation.frames.size() - 1);
    }
}

inline uint8_t* getAnimatedModFrameData(uint32_t hash) {
    if (auto* mod = getMod(hash)) {
        if (mod->isAnimated()) {
            auto& anim = mod->animation;
            if (anim.currentFrame < anim.frames.size()) {
                return anim.frames[anim.currentFrame].textureData.data();
            }
        }
    }
    return nullptr;
}

inline size_t getAnimatedModFrameSize(uint32_t hash) {
    if (auto* mod = getMod(hash)) {
        if (mod->isAnimated()) {
            auto& anim = mod->animation;
            if (anim.currentFrame < anim.frames.size()) {
                return anim.frames[anim.currentFrame].textureData.size();
            }
        }
    }
    return 0;
}
