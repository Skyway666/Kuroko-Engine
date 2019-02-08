#include "ResourceAnimation.h"
#include "Application.h"
#include "Globals.h"



ResourceAnimation::ResourceAnimation(resource_deff deff):Resource(deff)
{

}


ResourceAnimation::~ResourceAnimation()
{
}

void ResourceAnimation::LoadToMemory()
{
}

void ResourceAnimation::UnloadFromMemory()
{
}

void ResourceAnimation::resetFrames()
{
	for (int i = 0; i < numBones; i++)
	{
		boneTransformations[i].currentPosIndex = 0;
		boneTransformations[i].currentRotIndex = 0;
		boneTransformations[i].currentScaleIndex = 0;
	}
}

float ResourceAnimation::getDuration() const
{
	return ticks / ticksXsecond;
}

BoneTransform::~BoneTransform()
{
	RELEASE_ARRAY(PosKeysValues);
	RELEASE_ARRAY(PosKeysTimes);

	RELEASE_ARRAY(ScaleKeysValues);
	RELEASE_ARRAY(ScaleKeysTimes);

	RELEASE_ARRAY(RotKeysValues);
	RELEASE_ARRAY(RotKeysTimes);
}

bool BoneTransform::calcCurrentIndex(float time, bool test)
{
	bool ret = false;

	if ((/*!App->inGameMode() &&*/ !test) || currentPosIndex == -1 || currentRotIndex == -1 || currentScaleIndex == -1 ||
		nextPosIndex == -1 || nextRotIndex == -1 || nextScaleIndex == -1)
	{
		currentPosIndex = currentRotIndex = currentScaleIndex = 0;
		nextPosIndex = nextRotIndex = nextScaleIndex = 1;
		return true;
	}

	for (int i = 0; i < numPosKeys; i++)
	{
		if (PosKeysTimes[i] <= time && (i + 1 >= numPosKeys || PosKeysTimes[i + 1] > time))
		{
			currentPosIndex = i;

			nextPosIndex = currentPosIndex + 1;

			if (nextPosIndex >= numPosKeys && numPosKeys > 1)
			{
				nextPosIndex = 0;
			}


			ret = true;
			break;
		}
	}
	for (int i = 0; i < numRotKeys; i++)
	{
		if (RotKeysTimes[i] <= time && (i + 1 >= numRotKeys || RotKeysTimes[i + 1] > time))
		{
			currentRotIndex = i;

			nextRotIndex = currentRotIndex + 1;

			if (nextRotIndex >= numRotKeys && numRotKeys > 1)
			{
				nextRotIndex = 0;
			}

			ret = true;
			break;
		}
	}
	for (int i = 0; i < numScaleKeys; i++)
	{
		if (ScaleKeysTimes[i] <= time && (i + 1 >= numScaleKeys || ScaleKeysTimes[i + 1] > time))
		{
			currentScaleIndex = i;

			nextScaleIndex = currentScaleIndex + 1;

			if (nextScaleIndex >= numScaleKeys && numScaleKeys > 1)
			{
				nextScaleIndex = 0;
			}

			ret = true;
			break;
		}
	}

	return ret;
}

void BoneTransform::calcTransfrom(float time, bool interpolation)
{
	float tp, ts, tr;

	tp = ts = tr = 0.0f;

	float3 position_1 = { PosKeysValues[currentPosIndex * 3], PosKeysValues[currentPosIndex * 3 + 1], PosKeysValues[currentPosIndex * 3 + 2] };
	Quat rotation_1 = { RotKeysValues[currentRotIndex * 4], RotKeysValues[currentRotIndex * 4 + 1], RotKeysValues[currentRotIndex * 4 + 2], RotKeysValues[currentRotIndex * 4 + 3] };
	float3 scale_1 = { ScaleKeysValues[currentScaleIndex * 3], ScaleKeysValues[currentScaleIndex * 3 + 1], ScaleKeysValues[currentScaleIndex * 3 + 2] };

	float3 position_2 = { PosKeysValues[nextPosIndex * 3], PosKeysValues[nextPosIndex * 3 + 1], PosKeysValues[nextPosIndex * 3 + 2] };
	Quat rotation_2 = { RotKeysValues[nextRotIndex * 4], RotKeysValues[nextRotIndex * 4 + 1], RotKeysValues[nextRotIndex * 4 + 2], RotKeysValues[nextRotIndex * 4 + 3] };
	float3 scale_2 = { ScaleKeysValues[nextScaleIndex * 3], ScaleKeysValues[nextScaleIndex * 3 + 1], ScaleKeysValues[nextScaleIndex * 3 + 2] };


	tp = ((time - PosKeysTimes[currentPosIndex]) / (PosKeysTimes[nextPosIndex] - PosKeysTimes[currentPosIndex]));
	tr = ((time - RotKeysTimes[currentRotIndex]) / (RotKeysTimes[nextRotIndex] - RotKeysTimes[currentRotIndex]));
	ts = ((time - ScaleKeysTimes[currentScaleIndex]) / (ScaleKeysTimes[nextScaleIndex] - ScaleKeysTimes[currentScaleIndex]));

	tp = (tp < 0) ? 0 : tp;
	tr = (tr < 0) ? 0 : tr;
	ts = (ts < 0) ? 0 : ts;

	float3 position = position_1.Lerp(position_2, tp);
	Quat rotation = rotation_1.Slerp(rotation_2, tr);
	float3 scale = scale_1.Lerp(scale_2, ts);

	if (interpolation)
	{
		lastTransform.Set(float4x4::FromTRS(position, rotation, scale));
	}
	else
	{
		lastTransform.Set(float4x4::FromTRS(position_1, rotation_1, scale_1));
	}
}

void BoneTransform::smoothBlending(const float4x4& blendtrans, float time)
{
	float3 position_1;
	Quat rotation_1;
	float3 scale_1;

	lastTransform.Decompose(position_1, rotation_1, scale_1);

	float3 position_2;
	Quat rotation_2;
	float3 scale_2;

	blendtrans.Decompose(position_2, rotation_2, scale_2);

	float3 finalpos = position_1.Lerp(position_2, time);
	Quat finalrot = rotation_1.Slerp(rotation_2, time);
	float3 finalscale = scale_1.Lerp(scale_2, time);

	lastTransform.Set(float4x4::FromTRS(finalpos, finalrot, finalscale));
}
