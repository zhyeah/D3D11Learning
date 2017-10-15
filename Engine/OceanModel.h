#ifndef _OCEANMODEL_H_
#define _OCEANMODEL_H_

#include "SurfaceModel.h"
#include "MathHelper.h"
#include "FFT.h"

class OceanModel: public SurfaceModel
{
public:
	OceanModel();
	OceanModel(const OceanModel&);
	~OceanModel();

	virtual void Shutdown();
	virtual void UpdateModel(float t);

	bool Initialize(ID3D11Device* device, UINT gridCount, float length, float amp, float period, XMFLOAT2);
	void UpdateWaves(float t);
	void UpdateWavesWithFFT(float t);

protected:
	virtual bool InitializeBuffers(ID3D11Device* device);
	virtual void UpdateBuffers(ID3D11DeviceContext* deviceContext);

	float m_amp; //The amplitude of the wave;
	float m_g; //The gravity constant;
	float m_period; // The period of the wave;

	XMFLOAT2 m_windSpeed; //The speed vector of wind.
	XMFLOAT2* m_vertexTildeArray; // The tilde of vertexs.
	XMFLOAT2* m_vertexTildeConjArray; // The conj tilde of vertexs.

	FFT* m_fft; // FFT tool;
	XMFLOAT2 *m_htilde, // For fast fourier transform
		*m_htilde_slopex, *m_htilde_slopez,
		*m_htilde_dx, *m_htilde_dz;

	XMFLOAT3 *m_originPosition;

private:
	float GetDispersion(int n, int m); //Calculate the dispersion of wave.
	float GetPhilipsSpectrum(int n, int m); // Calculate philips spectrum as height of FT.
	XMFLOAT2 GetHeightTilde0(int n, int m); // Calculate the Height Tilde~0.
	XMFLOAT2 GetHeightTilde(float t, int n, int m); // Calculate the height tilde.
	void GetHDN(float t, XMFLOAT2 X, XMFLOAT2& height, XMFLOAT2& displacement, XMFLOAT3& normal); // Get the height, displacement, normal vector.
};

#endif