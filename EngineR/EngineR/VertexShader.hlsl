struct VSOut
{
	float4 color : Color;
	float4 pos   : Sv_Position;
};

cbuffer buffer
{
	row_major matrix transform;
};

VSOut main(float2 pos : Position, float4 color : Color)
{
	VSOut vso;
	vso.color = color;
	vso.pos = mul(float4(pos.x, pos.y, 0.0f, 1.0f),transform);
	return vso;
}