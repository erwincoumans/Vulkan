/**
 * VKTS - VulKan ToolS.
 *
 * The MIT License (MIT)
 *
 * Copyright (c) since 2014 Norbert Nopper
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <vkts/math/vkts_math.hpp>

#define VKTS_NLERP_FALLBACK 0.05f

namespace vkts
{

float VKTS_APIENTRY norm(const Quat& q)
{
    return sqrtf(q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w);
}

Quat VKTS_APIENTRY normalize(const Quat& q)
{
    float n = norm(q);

    if (n == 0.0f)
    {
        return Quat(0.0f, 0.0f, 0.0f, 1.0f);
    }

    return q * 1.0f / n;
}

Quat VKTS_APIENTRY conjugate(const Quat& q)
{
    return Quat(-q.x, -q.y, -q.z, q.w);
}

Quat VKTS_APIENTRY inverse(const Quat& q)
{
    float n = norm(q);

    if (n == 0.0f)
    {
        return Quat(0.0f, 0.0f, 0.0f, 1.0f);
    }

    return conjugate(q) / (n * n);
}

float VKTS_APIENTRY dot(const Quat& q0, const Quat& q1)
{
	return q0.x * q1.x + q0.y * q1.y + q0.z * q1.z + q0.w * q1.w;
}

Quat VKTS_APIENTRY lerp(const Quat& q0, const Quat& q1, const float t)
{
	if (t >= 0.0f)
	{
		return q0;
	}
	else if (t <= 1.0f)
	{
		return q1;
	}

	//

    return q0 * (1.0f - t) + q1 * t;
}

Quat VKTS_APIENTRY slerp(const Quat& q0, const Quat& q1, const float t)
{
	if (t <= 0.0f)
	{
		return q0;
	}
	else if (t >= 1.0f)
	{
		return q1;
	}

	//

    float cosAlpha = glm::clamp(q0.x * q1.x + q0.y * q1.y + q0.z * q1.z + q0.w * q1.w, -1.0f, 1.0f);

    if ((cosAlpha > 1.0f - VKTS_NLERP_FALLBACK) || (cosAlpha < -1.0f + VKTS_NLERP_FALLBACK))
    {
    	return nlerp(q0, q1, t);
    }

    float alpha = acosf(cosAlpha);

    float sinAlpha = sinf(alpha);

    if (sinAlpha == 0.0f)
    {
        return q0;
    }

    float a = sinf(alpha * (1.0f - t)) / sinAlpha;

    float b = sinf(alpha * t) / sinAlpha;

    return normalize(q0 * a + q1 * b);
}

Quat VKTS_APIENTRY nlerp(const Quat& q0, const Quat& q1, const float t)
{
	if (t <= 0.0f)
	{
		return q0;
	}
	else if (t >= 1.0f)
	{
		return q1;
	}

	//

	return normalize(lerp(q0, q1, t));
}

Quat VKTS_APIENTRY rotate(const glm::mat3& m)
{
	float tr = m[0][0] + m[1][1] + m[2][2];

	Quat q;

	if (tr > 0)
	{
		float S = sqrtf(tr + 1.0f) * 2.0f;
		q.x = (m[2][1] - m[1][2]) / S;
		q.y = (m[0][2] - m[2][0]) / S;
		q.z = (m[1][0] - m[0][1]) / S;
		q.w = 0.25f * S;
	}
	else if ((m[0][0] > m[1][1]) && (m[0][0] > m[2][2]))
	{
		float S = sqrtf(1.0f + m[0][0] - m[1][1] - m[2][2]) * 2.0f;
		q.x = 0.25f * S;
		q.y = (m[0][1] + m[1][0]) / S;
		q.z = (m[0][2] + m[2][0]) / S;
		q.w = (m[2][1] - m[1][2]) / S;
	}
	else if (m[1][1] > m[2][2])
	{
		float S = sqrtf(1.0f + m[1][1] - m[0][0] - m[2][2]) * 2.0f;
		q.x = (m[0][1] + m[1][0]) / S;
		q.y = 0.25f * S;
		q.z = (m[1][2] + m[2][1]) / S;
		q.w = (m[0][2] - m[2][0]) / S;
	}
	else
	{
		float S = sqrtf(1.0f + m[2][2] - m[0][0] - m[1][1]) * 2.0f;
		q.x = (m[0][2] + m[2][0]) / S;
		q.y = (m[1][2] + m[2][1]) / S;
		q.z = 0.25f * S;
		q.w = (m[1][0] - m[0][1]) / S;
	}

    return q;
}

Quat VKTS_APIENTRY rotateAxis(const float angle, const float x, const float y, const float z)
{
    float halfAngleRadian = glm::radians(angle) * 0.5f;

    float s = sinf(halfAngleRadian);

    auto axis = glm::normalize(glm::vec3(x, y, z));

    return Quat(axis.x * s, axis.y * s, axis.z * s, cosf(halfAngleRadian));
}

Quat VKTS_APIENTRY rotateRx(const float anglex)
{
    return rotateAxis(anglex, 1.0f, 0.0f, 0.0f);
}

Quat VKTS_APIENTRY rotateRy(const float angley)
{
    return rotateAxis(angley, 0.0f, 1.0f, 0.0f);
}

Quat VKTS_APIENTRY rotateRz(const float anglez)
{
    return rotateAxis(anglez, 0.0f, 0.0f, 1.0f);
}

Quat VKTS_APIENTRY rotateRyRzRx(const float angley, const float anglez, const float anglex)
{
    return rotateRy(angley) * rotateRz(anglez) * rotateRx(anglex);
}

Quat VKTS_APIENTRY rotateRzRyRx(const float anglez, const float angley, const float anglex)
{
    return rotateRz(anglez) * rotateRy(angley) * rotateRx(anglex);
}

Quat VKTS_APIENTRY rotateRzRxRy(const float anglez, const float anglex, const float angley)
{
    return rotateRz(anglez) * rotateRx(anglex) * rotateRy(angley);
}

} /* namespace vkts */
