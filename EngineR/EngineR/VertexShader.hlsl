struct VSOut
{
	float4 color : Color;
	float4 pos   : Sv_Position;
};

VSOut main(float2 pos : Position, float4 color : Color)
{
	VSOut vso;
	vso.color = color;
	vso.pos = float4(pos.x, pos.y, 0.0f, 1.0f);
	return vso;
}