//�O���[�o��
cbuffer global {
	//���[���h����ˉe�܂ł̕ϊ�
	matrix g_mWVP;
	float4 g_PolyColor;//�|���S���F
};


///////�o�[�e�b�N�X�V�F�[�_�[///////

float4 VS(float4 Pos : POSITION) : SV_POSITION{
	Pos = mul(Pos, g_mWVP);

	return Pos;
}

//++++++++++++++++++++++++++++++//


/////////�s�N�Z���V�F�[�_�[/////////

float4 PS(float4 Pos : SV_POSITION) : SV_Target{
	return g_PolyColor;
}

//+++++++++++++++++++++++++++++++//