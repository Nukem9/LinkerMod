#include <Windows.h>
#include "techniques.h"

/* 1264 */
struct GfxDrawSurfFields
{
  __int64 _bf0;
};

/* 1265 */
union GfxDrawSurf
{
  GfxDrawSurfFields fields;
  unsigned __int64 packed;
};

/* 1266 */
struct MaterialInfo
{
  const char *name;
  unsigned int gameFlags;
  char pad;
  char sortKey;
  char textureAtlasRowCount;
  char textureAtlasColumnCount;
  GfxDrawSurf drawSurf;
  unsigned int surfaceTypeBits;
  unsigned int layeredSurfaceTypes;
  unsigned __int16 hashIndex;
};

/* 1275 */
struct IDirect3DPixelShader9
{
  DWORD baseclass_0;
};

/* 1276 */
struct GfxPixelShaderLoadDef
{
  unsigned int *program;
  unsigned __int16 programSize;
};

/* 1277 */
struct MaterialPixelShaderProgram
{
  IDirect3DPixelShader9 *ps;
  GfxPixelShaderLoadDef loadDef;
};

/* 1278 */
struct MaterialPixelShader
{
  const char *name;
  MaterialPixelShaderProgram prog;
};

/* 1282 */
union $3E38D60757422ABD074F0BB8A7F768B6
{
  MaterialPixelShader *pixelShader;
  MaterialPixelShader *localPixelShader;
};

/* 1279 */
struct MaterialArgumentCodeConst
{
  unsigned __int16 index;
  char firstRow;
  char rowCount;
};

/* 1280 */
union MaterialArgumentDef
{
  const float *literalConst;
  MaterialArgumentCodeConst codeConst;
  unsigned int codeSampler;
  unsigned int nameHash;
};

/* 1281 */
struct MaterialShaderArgument
{
  unsigned __int16 type;
  unsigned __int16 dest;
  MaterialArgumentDef u;
};

/* 1283 */
union $835B54ED439E881A12CFCCFD6075577F
{
  MaterialShaderArgument *localArgs;
  MaterialShaderArgument *args;
};

/* 1268 */
struct IDirect3DVertexDeclaration9
{
  DWORD baseclass_0;
};

/* 1267 */
struct MaterialStreamRouting
{
  char source;
  char dest;
};

/* 1269 */
struct MaterialVertexStreamRouting
{
  MaterialStreamRouting data[16];
  IDirect3DVertexDeclaration9 *decl[18];
};

/* 1270 */
struct MaterialVertexDeclaration
{
  char streamCount;
  bool hasOptionalSource;
  bool isLoaded;
  MaterialVertexStreamRouting routing;
};

/* 1271 */
struct IDirect3DVertexShader9
{
  DWORD baseclass_0;
};

/* 1272 */
struct GfxVertexShaderLoadDef
{
  unsigned int *program;
  unsigned __int16 programSize;
};

/* 1273 */
struct MaterialVertexShaderProgram
{
  IDirect3DVertexShader9 *vs;
  GfxVertexShaderLoadDef loadDef;
};

/* 1274 */
struct MaterialVertexShader
{
  const char *name;
  MaterialVertexShaderProgram prog;
};

/* 1284 */
struct MaterialPass
{
  MaterialVertexDeclaration *vertexDecl;
  MaterialVertexShader *vertexShader;
  $3E38D60757422ABD074F0BB8A7F768B6 ___u2;
  char perPrimArgCount;
  char perObjArgCount;
  char stableArgCount;
  char customSamplerFlags;
  $835B54ED439E881A12CFCCFD6075577F ___u7;
};

/* 1285 */
struct MaterialTechnique
{
  const char *name;
  unsigned __int16 flags;
  unsigned __int16 passCount;
  MaterialPass passArray[1];
};

/* 1286 */
struct MaterialTechniqueSet
{
  const char *name;
  char worldVertFormat;
  char unused[1];
  unsigned __int16 techsetFlags;
  MaterialTechnique *techniques[130];
};

/* 1303 */
union $6ADFD931CD5BA920F612AF505CA16E3E
{
  MaterialTechniqueSet *localTechniqueSet;
  MaterialTechniqueSet *techniqueSet;
};

/* 1301 */
struct MaterialConstantDef
{
  unsigned int nameHash;
  char name[12];
  float literal[4];
};

/* 1304 */
union $C3F7A89422F64E7CE7B1D04043902CE5
{
  MaterialConstantDef *localConstantTable;
  MaterialConstantDef *constantTable;
};

/* 1299 */
union MaterialTextureDefInfo
{
  /*GfxImage*/void *image;
  /*water_t*/void *water;
};

/* 1300 */
struct MaterialTextureDef
{
  unsigned int nameHash;
  char nameStart;
  char nameEnd;
  char samplerState;
  char semantic;
  char isMatureContent;
  char pad[3];
  MaterialTextureDefInfo u;
};

/* 1302 */
struct GfxStateBits
{
  unsigned int loadBits[2];
};

/* 1305 */
struct Material
{
  MaterialInfo info;
  char stateBitsEntry[130];
  char textureCount;
  char constantCount;
  char stateBitsCount;
  char stateFlags;
  char cameraRegion;
  char maxStreamedMips;
  $6ADFD931CD5BA920F612AF505CA16E3E ___u8;
  MaterialTextureDef *textureTable;
  $C3F7A89422F64E7CE7B1D04043902CE5 ___u10;
  GfxStateBits *stateBitsTable;
};