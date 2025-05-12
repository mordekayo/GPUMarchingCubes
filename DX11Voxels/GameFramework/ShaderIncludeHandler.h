#pragma once
#include "Exports.h"
#include "d3d11.h"

class GAMEFRAMEWORK_API ShaderIncludeHandler : public ID3DInclude
{
public:
	virtual HRESULT Open(D3D_INCLUDE_TYPE IncludeType, LPCSTR pFileName, LPCVOID pParentData, LPCVOID* ppData, UINT* pBytes) override;
	virtual HRESULT Close(LPCVOID pData) override;


	//	public void Close(Stream stream)
	//{
	//	stream.Close();
	//}
	//
	//
	//public Stream Open(IncludeType type, string fileName, Stream parentStream)
	//{
	//	var path = Path.Combine("./Shaders", fileName);
	//	if (!File.Exists(path)) throw new FileNotFoundException("File not found at: " + path);
	//
	//	return File.OpenRead(path);
	//}
};

