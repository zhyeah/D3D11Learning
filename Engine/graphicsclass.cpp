#include "graphicsclass.h"
#include "EngineConfig.h"
using namespace EngineConfig;

GraphicsClass::GraphicsClass()
{
	m_totalTimeElapse = 0.0f;
	m_direct3D = NULL;
	m_camera = NULL;
	m_model = NULL;
	m_mirror = NULL;
	m_ocean = NULL;
	m_shader = NULL;
}

GraphicsClass::GraphicsClass(const GraphicsClass &)
{
}

GraphicsClass::~GraphicsClass()
{
}

bool GraphicsClass::Initialize(int screenWidth, int screenHeight, HWND hwnd)
{
	m_direct3D = new D3DClass();
	if (!m_direct3D) {
		return false;
	}

	bool result = m_direct3D->Initialize(
		screenWidth, screenHeight, 
		DisplayConfig::UseVSync, 
		hwnd, 
		DisplayConfig::UseFullScreen, 
		DisplayConfig::ScreenNearPlaen, 
		DisplayConfig::ScreenFarPlane);

	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize Direct3D", L"Error", MB_OK);
		return false;
	}

	// Create the camera object.
	m_camera = new Camera;
	if (!m_camera)
	{
		return false;
	}

	// Set the initial position of the camera.
	m_camera->SetPosition(-30.0f, 30.0f, -30.0f);

	// Create the model object.
	m_model = new SphereModel;
	if (!m_model)
	{
		return false;
	}

	m_mirror = new RectangleModel;
	if (!m_mirror)
	{
		return false;
	}

	m_ocean = new OceanModel;
	if (!m_ocean)
	{
		return false;
	}

	// Initialize the model object.
	result = m_model->Initialize(m_direct3D->GetDevice(), 1.0f, 24, 36, PathManager::GetTextureFilePath(L"earth.jpg"));
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the model object.", L"Error", MB_OK);
		return false;
	}

	// Set model material.
	m_model->SetMaterial(
		XMVectorSet(0.1f, 0.2f, 0.3f, 1.0f),
		XMVectorSet(0.4f, 0.6f, 0.3f, 1.0f),
		XMVectorSet(0.7f, 0.7f, 0.9f, 1.0f),
		XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f)
	);

	result = m_mirror->Initialize(m_direct3D->GetDevice(), 5.0f, 3.0f);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the model object.", L"Error", MB_OK);
		return false;
	}

	m_mirror->SetMaterial(
		XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f),
		XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f),
		XMVectorSet(0.9f, 0.9f, 0.9f, 1.0f),
		XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f)
	);

	// Set model position.
	m_mirror->SetRotation(0.0f, XM_PIDIV4, 0.0f);
	m_mirror->SetPosition(XMVectorSet(3.0f, 0.0f, 3.0f, 1.0f));

	result = m_ocean->Initialize(m_direct3D->GetDevice(), 63, 64.0f, 0.0002f, 200.0f, XMFLOAT2(0.0f, 16.0f));
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the model object.", L"Error", MB_OK);
		return false;
	}

	m_ocean->SetMaterial(
		XMVectorSet(0.01f, 0.01f, 0.02f, 1.0f),
		XMVectorSet(0.1f, 0.3f, 0.7f, 1.0f),
		XMVectorSet(0.4f, 0.4f, 0.8f, 10.0f),
		XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f)
	);

	// Set model position.
	m_ocean->SetPosition(XMVectorSet(0.0f, -5.0f, 0.0f, 1.0f));

	// Create the shader object.
	m_shader = new NormalShader;
	if (!m_shader)
	{
		return false;
	}

	// Initialize the shader object.
	LPWCH vsPath = PathManager::GetShaderFilePath(L"Lambert.vsh");
	LPWCH psPath = PathManager::GetShaderFilePath(L"Lambert.psh");
	result = m_shader->Initialize(m_direct3D->GetDevice(), hwnd,
		vsPath, "VS",
		psPath, "PS");
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the color shader object.", L"Error", MB_OK);
		return false;
	}

	// Release resource.
	delete[] vsPath;
	vsPath = NULL;
	delete[] psPath;
	psPath = NULL;

	// Set lighat direction.
	DirectionalLight directionalLight;
	PointLight pointLight;
	SpotLight spotLight;

	InitializeLight(directionalLight, pointLight, spotLight);
	m_shader->SetDirectionalLight(directionalLight);
	m_shader->SetPointLight(pointLight);
	m_shader->SetSpotLight(spotLight);
	m_shader->SetEyePosition(m_camera->GetPosition());

	return true;
}

void GraphicsClass::Shutdown()
{
	// Release the color shader object.
	if (m_shader)
	{
		m_shader->Shutdown();
		delete m_shader;
		m_shader = NULL;
	}

	// Release the model object.
	if (m_model)
	{
		m_model->Shutdown();
		delete m_model;
		m_model = NULL;
	}

	if (m_mirror)
	{
		m_mirror->Shutdown();
		delete m_mirror;
		m_mirror = NULL;
	}

	if (m_ocean)
	{
		m_ocean->Shutdown();
		delete m_ocean;
		m_ocean = NULL;
	}

	// Release the camera object.
	if (m_camera)
	{
		delete m_camera;
		m_camera = NULL;
	}

	if (m_direct3D) 
	{
		m_direct3D->Shutdown();
		delete m_direct3D;
		m_direct3D = NULL;
	}
}

bool GraphicsClass::Frame(float dt)
{
	m_totalTimeElapse += dt;
	Render(dt);

	return true;
}

bool GraphicsClass::Render(float dt)
{
	XMMATRIX viewMatrix, projectionMatrix;
	bool result;

	/*                                                */
	/* Render all the things except mirror to texture.*/
	/*                                                */

	// Switch to render to texture.
	ID3D11ShaderResourceView* shaderResourceView = NULL;
	m_direct3D->RenderToTexture(DisplayConfig::ScreenWidth, DisplayConfig::ScreenHeight, &shaderResourceView);
	// Clear the buffers to begin the scene.
	m_direct3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);
	
	XMVECTOR planar = XMPlaneNormalize(XMVectorSet(-1, 0, -1, 6));
	XMMATRIX reflect = XMMatrixReflect(planar);

	// Generate the view matrix based on the camera's position.
	m_camera->Render();

	// Reflect position.
	XMFLOAT3 pos = m_camera->GetPosition();
	XMVECTOR positionVector = XMVector3TransformCoord(XMLoadFloat3(&pos), reflect);

	// Reflect look at point.
	XMFLOAT3 lookAt = m_camera->GetLookAt();
	XMVECTOR lookAtVector = XMVector3TransformCoord(XMLoadFloat3(&lookAt), reflect);

	XMFLOAT3 up = XMFLOAT3(0.0f, 1.0f, 0.0f);
	XMMATRIX reflectViewMatrix = XMMatrixLookAtLH(positionVector, lookAtVector, XMLoadFloat3(&up));

	
	m_direct3D->GetProjectionMatrix(projectionMatrix);
	
	// Set view and project matrix.
	m_shader->SetViewMatrix(reflectViewMatrix);
	m_shader->SetProjectMatrix(projectionMatrix);

	// Draw sphere model.
	m_model->Render(m_direct3D->GetDeviceContext(), m_shader);

	/*                                                           */
	/* Now we get the texture rendered, just draw as camera sit. */
	/*                                                           */
	m_direct3D->RenderToWindow();
	m_direct3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	// Get the world, view, and projection matrices from the camera and d3d objects.
	m_camera->GetViewMatrix(viewMatrix);

	// Set view and project matrix.
	m_shader->SetViewMatrix(viewMatrix);
	m_shader->SetProjectMatrix(projectionMatrix);

	// Draw sphere model.
	m_model->Render(m_direct3D->GetDeviceContext(), m_shader);

	// Draw mirror model.
	m_mirror->SetTexture(shaderResourceView);
	m_mirror->Render(m_direct3D->GetDeviceContext(), m_shader);

	// Draw ocean.
	m_ocean->UpdateModel(m_totalTimeElapse);
	m_ocean->Render(m_direct3D->GetDeviceContext(), m_shader);

	m_direct3D->EndScene();

	return true;
}

void GraphicsClass::InitializeLight(DirectionalLight& directionalLight, PointLight& pointLight, SpotLight& spotLight)
{
	// Initialize directional light.
	directionalLight.ambient = XMVectorSet(0.1f, 0.1f, 0.1f, 1.0f);
	directionalLight.diffuse = XMVectorSet(0.4f, 0.4f, 0.4f, 1.0f);
    directionalLight.specular = XMVectorSet(0.9f, 0.9f, 0.9f, 1.0f);
	directionalLight.direction = XMVector4Normalize(XMVectorSet(0.0f, -1.0f, 1.0f, 0.0f));

	// Initialize point light.
	pointLight.ambient = XMVectorSet(0.3f, 0.3f, 0.3f, 1.0f);
	pointLight.diffuse = XMVectorSet(0.6f, 0.6f, 0.6f, 1.0f);
	pointLight.specular = XMVectorSet(0.9f, 0.9f, 0.9f, 1.0f);
	pointLight.position = XMFLOAT3(2.5f, 0.0f, 2.5f);
	pointLight.range = 5.0f;
	pointLight.att = XMVectorSet(0.0f, 2.0f, 0.0f, 0.0f);

	// Initialize spot light.
	spotLight.ambient = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
	spotLight.diffuse = XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f);
	spotLight.specular = XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f);
	spotLight.position = XMFLOAT3(900.0f, 900.0f, -900.0f);
	spotLight.range = 1.0f;
	spotLight.direction = XMFLOAT3(-1.0f, -1.0f, 1.0f);
	spotLight.spot = 90.0f;
	spotLight.att = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
}
