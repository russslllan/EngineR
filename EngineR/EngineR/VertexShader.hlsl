
cbuffer buffer
{
	matrix transform;
};

float4 main(float3 pos : Position) : Sv_Position
{
	float4 res;
	res = mul(float4(pos, 1.0f),transform);
	return res;
}