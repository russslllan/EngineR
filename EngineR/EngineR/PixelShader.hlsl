cbuffer cbu
{
	float4 color[6];
};

float4 main(uint tid : Sv_PrimitiveID) : Sv_Target
{
	return color[tid/2];
}