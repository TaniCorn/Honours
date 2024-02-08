#include "BaseComputeShader.h"

////////////////////TANPAT SOMRID
////////////////////HOW TO USE:
/*Your Guide to the amazing world of pain(lessened now)
* Cbuffers are set up the usual way. They can be updated with ID3D11DeviceContext.UpdateSubresource and ID3D11DeviceContext.CSSetConstantBuffer
* Keep in mind for the next two things. Both SRV and UAV need a ID3D11Buffer to be bound to them. However do not bind a single ID3D11Buffer to a UAV and SRV. Only the UAV will take place.
* 
* A read only structure in compute shaders is set up as StructuredBuffer<something> Input : register(t0).
*   In C++ They are known as SRV - Shader Resource View. Imagine it as being a texture. 
*   To update it ID3D11Buffer->CSSetShaderResources(0, 1, &SRV), this will bind your current SRV(your buffer representation of it) to the shader
* 
* A Read-Write structure in compute shaders is set up as RWStructuredBuffer<something> Output : register(u0);
*   In C++ They are known as UAV - Unordered Access View.
*   To update it ID3D11Buffer->CSSetUnorderedAccessViews(0, 1, &UAV, nullptr);
* 
* To get data from GPU memory to CPU Memory we must have a read back buffer in C++. 
*   We then call ID3D11DeviceContext.CopyResource(readBuffer, buffer); //Buffer being the one bound to the UAV, but not the UAV shader resource itself
* 
*   After that we can Dispatch with xyz amount of threads. This is a whole other ball park.
    I'm not confident enough to guide with this one. Just experiment, think about why it's not working and figure it out.

    Good luck, I don't know why I'm writing this to myself. Probably because I've driven myself crazy with this.
*/

void BaseComputeShader::CreateComputeShader(const wchar_t* computeShaderName, const char* computeShaderEntryPoint, ID3D11ComputeShader** computeShaderOut)
{
    HRESULT hr = S_OK;
    ID3DBlob* pBlob = nullptr;

    //Compile the shader
    hr = CompileShaderFromFile(computeShaderName, computeShaderEntryPoint, "cs_4_0", &pBlob);
    assert(hr == S_OK);
    //output the compiled shader into our shader
    hr = device->CreateComputeShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, computeShaderOut);
    assert(hr == S_OK);

    SAFE_RELEASE(pBlob);
   
    // Private data is simply debugging data
#if defined(_DEBUG)
    (*computeShaderOut)->SetPrivateData(WKPDID_D3DDebugObjectName, sizeof(computeShaderName) - 1, computeShaderName);
#endif  
}


void BaseComputeShader::CreateComputeData()
{
    HRESULT hr = S_OK;

    hr = CreateConstantBuffer(&g_pCB, sizeof(CB));
    assert(hr == S_OK);

    // Create the Buffer of Elements
    D3D11_BUFFER_DESC buffer_desc = {};
    buffer_desc.ByteWidth = resolution * sizeof(float);
    buffer_desc.Usage = D3D11_USAGE_DEFAULT;
    buffer_desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
    buffer_desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
    buffer_desc.StructureByteStride = sizeof(float);
    hr = device->CreateBuffer(&buffer_desc, nullptr, &g_pBuffer1);
#if defined(_DEBUG)
    g_pBuffer1->SetPrivateData(WKPDID_D3DDebugObjectName, sizeof("Buffer1") - 1, "Buffer1");
#endif
    device->CreateBuffer(&buffer_desc, nullptr, &g_pBuffer2);
    //    //This is the readonly of buffer 1, why does it not have data?
    //    // Create the Shader Resource View for the Buffers
    //    // This is used for reading the buffer during the transpose
    D3D11_SHADER_RESOURCE_VIEW_DESC srvbuffer_desc = {};
    srvbuffer_desc.Format = DXGI_FORMAT_UNKNOWN;
    srvbuffer_desc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
    srvbuffer_desc.Buffer.ElementWidth = resolution;
    hr = device->CreateShaderResourceView(g_pBuffer1, &srvbuffer_desc, &g_pBufferSRV);
//#if defined(_DEBUG)
//    g_pBuffer1SRV->SetPrivateData(WKPDID_D3DDebugObjectName, sizeof("Buffer1 SRV") - 1, "Buffer1 SRV");
//#endif
    //This is the RW of buffer1
    // Create the Unordered Access View for the Buffers
    // This is used for writing the buffer during the sort and transpose
    D3D11_UNORDERED_ACCESS_VIEW_DESC uavbuffer_desc = {};
    uavbuffer_desc.Format = DXGI_FORMAT_UNKNOWN;
    uavbuffer_desc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
    uavbuffer_desc.Buffer.NumElements = resolution;
    hr = device->CreateUnorderedAccessView(g_pBuffer1, &uavbuffer_desc, &g_pBufferUAV);

#if defined(_DEBUG)
    g_pBufferUAV->SetPrivateData(WKPDID_D3DDebugObjectName, sizeof("Buffer1 UAV") - 1, "Buffer1 UAV");
#endif
    CreateReadBackBuffer(&g_pReadBackBuffer);
}


HRESULT BaseComputeShader::CreateConstantBuffer(ID3D11Buffer** outBuffer, UINT byteWidth)
{
    HRESULT hr = S_OK;

    // Create the Const Buffer
    D3D11_BUFFER_DESC constant_buffer_desc = {};
    constant_buffer_desc.ByteWidth = byteWidth;
    constant_buffer_desc.Usage = D3D11_USAGE_DEFAULT;
    constant_buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    constant_buffer_desc.CPUAccessFlags = 0;
    hr = device->CreateBuffer(&constant_buffer_desc, nullptr, outBuffer);
#if defined(_DEBUG)
    (*outBuffer)->SetPrivateData(WKPDID_D3DDebugObjectName, sizeof("Constant Buffer") - 1, "Constant Buffer");//We can modify the function later to pass in a string to give each constant buffer a name. But for now it's unecessary
#endif
    return hr;
}
HRESULT BaseComputeShader::CreateReadBackBuffer(ID3D11Buffer** readBuffer)
{
    HRESULT hr = S_OK;
    // Create the Readback Buffer
    //This is used to read the results back to the CPU
    D3D11_BUFFER_DESC readback_buffer_desc = {};
    readback_buffer_desc.ByteWidth = resolution * sizeof(float);
    readback_buffer_desc.Usage = D3D11_USAGE_STAGING;
    readback_buffer_desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
    readback_buffer_desc.StructureByteStride = sizeof(float);
    hr = device->CreateBuffer(&readback_buffer_desc, nullptr, &g_pReadBackBuffer);
    #if defined(_DEBUG)
        g_pReadBackBuffer->SetPrivateData(WKPDID_D3DDebugObjectName, sizeof("ReadBack Buffer") - 1, "ReadBack Buffer");
    #endif
    return hr;
}


void BaseComputeShader::GPUSort(ID3D11DeviceContext* currentContext)
{
    fin = false;
    // Upload the data
    currentContext->UpdateSubresource(g_pBuffer2, 0, nullptr, &data[0], 0, 0);//puts all of data into buffer1

    // Sort the row data
    currentContext->CSSetShaderResources(0, 1, &g_pBufferSRV);
    currentContext->CSSetUnorderedAccessViews(0, 1, &g_pBufferUAV, nullptr);
    currentContext->CSSetShader(g_pComputeShaderBitonic, nullptr, 0);
    currentContext->Dispatch(1, 1, 1);
    //for (int i = 0; i < NUM_ELEMENTS; i++)
    //{
    //    SetConstants(currentContext, i, 1, MATRIX_HEIGHT, MATRIX_WIDTH);
    //    currentContext->Dispatch(1, 1, 1);

    //}

// Download the data
    D3D11_MAPPED_SUBRESOURCE MappedResource = { 0 };
    currentContext->CopyResource(g_pReadBackBuffer, g_pBuffer1);
    if (SUCCEEDED(currentContext->Map(g_pReadBackBuffer, 0, D3D11_MAP_READ, 0, &MappedResource)))
    {
        _Analysis_assume_(MappedResource.pData);
        assert(MappedResource.pData);
        memcpy(&results[0], MappedResource.pData, resolution * sizeof(float));
        currentContext->Unmap(g_pReadBackBuffer, 0);

    }
    fin = true;
    std::vector<float> fl;
    for (int i = 0; i < resolution; i++)
    {
        fl.push_back(results[i]);
    }
    CopyResultsIntoData();
}

/// These function are completely grabbed off https://github.com/walbourn/directx-sdk-samples/tree/main/BasicCompute11
/// I do not care to write my own file opener or shader compiler
HRESULT BaseComputeShader::FindDXSDKShaderFileCch(WCHAR* strDestPath, int cchDest, LPCWSTR strFilename)
{
    if (!strFilename || strFilename[0] == 0 || !strDestPath || cchDest < 10)
        return E_INVALIDARG;

    // Get the exe name, and exe path
    WCHAR strExePath[MAX_PATH] =
    {
        0
    };
    WCHAR strExeName[MAX_PATH] =
    {
        0
    };
    WCHAR* strLastSlash = nullptr;
    GetModuleFileName(nullptr, strExePath, MAX_PATH);
    strExePath[MAX_PATH - 1] = 0;
    strLastSlash = wcsrchr(strExePath, TEXT('\\'));
    if (strLastSlash)
    {
        wcscpy_s(strExeName, MAX_PATH, &strLastSlash[1]);

        // Chop the exe name from the exe path
        *strLastSlash = 0;

        // Chop the .exe from the exe name
        strLastSlash = wcsrchr(strExeName, TEXT('.'));
        if (strLastSlash)
            *strLastSlash = 0;
    }

    // Search in directories:
    //      .\
        //      %EXE_DIR%\..\..\%EXE_NAME%

    wcscpy_s(strDestPath, cchDest, strFilename);
    if (GetFileAttributes(strDestPath) != 0xFFFFFFFF)
        return S_OK;

    swprintf_s(strDestPath, cchDest, L"%s\\..\\..\\%s\\%s", strExePath, strExeName, strFilename);
    if (GetFileAttributes(strDestPath) != 0xFFFFFFFF)
        return S_OK;

    // On failure, return the file as the path but also return an error code
    wcscpy_s(strDestPath, cchDest, strFilename);

    return E_FAIL;
}
HRESULT BaseComputeShader::CompileShaderFromFile(const WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut)
{
    if (!ppBlobOut)
        return E_INVALIDARG;

    // Find the hlsl file
    WCHAR str[MAX_PATH];
    HRESULT hr = FindDXSDKShaderFileCch(str, MAX_PATH, szFileName);
    if (FAILED(hr))
        return hr;

    DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;

    ID3DBlob* pErrorBlob = nullptr;
    hr = D3DCompileFromFile(str, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, szEntryPoint, szShaderModel, dwShaderFlags, 0, ppBlobOut, &pErrorBlob);
    if (FAILED(hr))
    {
        if (pErrorBlob)
            OutputDebugStringA((char*)pErrorBlob->GetBufferPointer());

        SAFE_RELEASE(pErrorBlob);

        return hr;
    }

    SAFE_RELEASE(pErrorBlob);

    return S_OK;

}