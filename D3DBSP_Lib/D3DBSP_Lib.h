#ifdef D3DBSP_LIB_EXPORTS
#define D3DBSP_LIB_API __declspec(dllexport)
#else
#define D3DBSP_LIB_API __declspec(dllimport)
#endif
