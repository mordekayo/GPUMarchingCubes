#include "ShaderIncludeHandler.h"
#include <string>
#include <cassert>
#include <fstream>


HRESULT ShaderIncludeHandler::Open(D3D_INCLUDE_TYPE IncludeType, LPCSTR pFileName, LPCVOID pParentData, LPCVOID* ppData, UINT* pBytes)
{
	//try
	{
		/*
		If pFileName is absolute: finalPath = pFileName.
		If pFileName is relative: finalPath = dir + "\\" + pFileName
		*/
		std::string finalPath;
		// switch (IncludeType)
		// {
		// case D3D_INCLUDE_LOCAL: // #include "FILE"
		//     common::RelativeToAbsolutePath(&finalPath, m_ShaderDir, pFileName);
		//     break;
		// case D3D_INCLUDE_SYSTEM: // #include <FILE>
		//     common::RelativeToAbsolutePath(&finalPath, m_SystemDir, pFileName);
		//     break;
		// default:
		//     assert(0);
		// }
		//

		std::string fileName(pFileName);
		std::string fullPath = "./Shaders/" + fileName;

		std::ifstream fileStream(fullPath, std::ios::in | std::ios::binary | std::ios::ate);

		if (fileStream.is_open()) {
			auto fileSize = (size_t)fileStream.tellg();
			char* buf = new char[fileSize];
			fileStream.seekg(0, std::ios::beg);
			
			fileStream.read(buf, fileSize);
			fileStream.close();

			*ppData = buf;
			*pBytes = fileSize;
		}
		else
		{
			*ppData = nullptr;
			*pBytes = 0;
		}
		
		return S_OK;
	}
	//catch (std::error& err)
	//{
	//	
	//	return E_FAIL;
	//}
}


HRESULT ShaderIncludeHandler::Close(LPCVOID pData)
{
    // Here we must correctly free buffer created in Open.
    char* buf = (char*)pData;
    delete[] buf;
    return S_OK;
}
