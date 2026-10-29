import Attributes;

struct PushMatrices
{
	float4x4 mvp;
};
[PushConstant]
[[vk::push_constant]]
ConstantBuffer<PushMatrices> pc;

struct VS_IN
{
	float2 position : POSITION;
	float2 uv : TEXTCOORD; // the position relative to the shape's center, in the same units as ubo.halfSize
};

struct VS_OUT
{
	float4 position : SV_Position;
	float2 local : TEXTCOORD;
};

VS_OUT VertexMain(VS_IN input)
{
	VS_OUT output;
	output.position = mul(pc.mvp, float4(input.position, 0.0, 1.0));
	output.local = input.uv;
	return output;
}

struct UBO
{
	float4 fillColor;
	float4 borderColor;
	float4 shadowColor;
	float4 depthColor;
	float4 cornerRadii; // top left, top right, bottom right, bottom left
	float2 halfSize;
	float2 shadowOffset;
	float2 shadowBlur;      // on each axis
	float2 shadowDirection; // on each axis, -1, 1, or 0 for both ways (see BlurredSegment)
	float  borderWidth;
	float  shadowSpread;
	float  depth;
	float  press; // how far the face is pushed down into the depth, at most the depth
};

struct Material
{
	ConstantBuffer<UBO> ubo;
};
ParameterBlock<Material> material;

// Signed distance to a rect centered on the origin (Y-up), each corner rounded by its own radius: negative inside
float RoundedRectDistance(float2 p, float2 halfSize, float4 cornerRadii)
{
	float2 sideRadii = (p.x > 0.0) ? cornerRadii.yz : cornerRadii.xw; // (top, bottom) of the side p is on
	float  radius = (p.y > 0.0) ? sideRadii.x : sideRadii.y;
	float2 q = abs(p) - halfSize + radius;
	return min(max(q.x, q.y), 0.0) + length(max(q, 0.0)) - radius;
}

// Abramowitz & Stegun 7.1.26, max error 1.5e-7
float Erf(float x)
{
	float s = sign(x);
	x = abs(x);
	float t = 1.0 / (1.0 + 0.3275911 * x);
	float y = 1.0 - (((((1.061405429 * t - 1.453152027) * t) + 1.421413741) * t - 0.284496736) * t + 0.254829592) * t * exp(-x * x);
	return s * y;
}

static const float Pi = 3.14159265358979;

float Gaussian(float x, float sigma)
{
	return exp(-(x * x) / (2.0 * sigma * sigma)) / (sqrt(2.0 * Pi) * sigma);
}

// The integral of the normalized gaussian of `sigma` from -infinity to x
float GaussianIntegral(float x, float sigma)
{
	return 0.5 + 0.5 * Erf(x / (sigma * sqrt(2.0)));
}

// How much of the segment [low, high] of an axis is at x once blurred along it by a gaussian. `direction` 0: the whole gaussian, as
// a regular blur. Otherwise only its half on the side opposite to the direction, doubled: the segment is smeared toward the
// direction only, it fades on that side and keeps its sharp edge on the other one
float BlurredSegment(float x, float low, float high, float sigma, float direction)
{
	if (direction == 0.0)
	{
		return GaussianIntegral(high - x, sigma) - GaussianIntegral(low - x, sigma);
	}
	// The distances t >= 0 from x, toward the segment, whose sample x - direction * t is in it
	float near = max((direction > 0.0) ? x - high : low - x, 0.0);
	float far = (direction > 0.0) ? x - low : high - x;
	return (far > near) ? 2.0 * (GaussianIntegral(far, sigma) - GaussianIntegral(near, sigma)) : 0.0;
}

// The row at height y of a rounded rect centered on the origin, blurred along x: how much of it is at x. The row goes from the left
// side to the right one, or less where it crosses a rounded corner
float BlurredRow(float x, float y, float2 halfSize, float4 cornerRadii, float sigma, float direction)
{
	float2 radii = (y > 0.0) ? cornerRadii.xy : cornerRadii.wz; // (left, right) corners on the row's side
	float2 cornerY = min(halfSize.y - radii - abs(y), 0.0);    // how far into each corner the row is, 0 out of them
	float2 ends = halfSize.x - radii + sqrt(max(radii * radii - cornerY * cornerY, 0.0));
	return BlurredSegment(x, -ends.x, ends.y, sigma, direction);
}

// A rounded rect centered on the origin (top left, top right, bottom right, bottom left radii) blurred by a gaussian, its own sigma
// and direction (see BlurredSegment) on each axis: how much of it is at p. Evan Wallace's method: exact along x, integrated along y
// over the rows the gaussian has weight on. The sampled axis has to be the less blurred one, or its few rows show as bands across
// the corners: the caller swaps the axes when x is
float BlurredRoundedRect(float2 p, float2 halfSize, float4 cornerRadii, float2 sigma, float2 direction)
{
	// The offsets y of the rows p.y - y the gaussian has weight on, within the rect. Directional, only those on the side opposite to
	// the direction, the weights doubled
	float start = (direction.y > 0.0) ? 0.0 : -3.0 * sigma.y;
	float end = (direction.y < 0.0) ? 0.0 : 3.0 * sigma.y;
	start = clamp(start, p.y - halfSize.y, p.y + halfSize.y);
	end = clamp(end, p.y - halfSize.y, p.y + halfSize.y);
	float weight = (direction.y != 0.0) ? 2.0 : 1.0;

	static const int SampleCount = 8;
	float            step = (end - start) / SampleCount;
	float            y = start + 0.5 * step;
	float            value = 0.0;
	for (int i = 0; i < SampleCount; ++i)
	{
		value += BlurredRow(p.x, p.y - y, halfSize, cornerRadii, sigma.x, direction.x) * Gaussian(y, sigma.y) * step;
		y += step;
	}
	return weight * value;
}

// How much of a pixel `distance` (to an outline, negative inside) is covered, the edge smoothed over one pixel of `pixelSize` units.
// The size comes from the position, not from the derivatives of the distance: those break where the closest side changes (a 2x2 pixel
// quad across a square corner sees both), which blurs the corners
float Coverage(float distance, float pixelSize)
{
	return saturate(0.5 - distance / pixelSize);
}

float4 FragmentMain(VS_OUT input) : SV_Target
{
	float2 halfSize = material.ubo.halfSize;
	// A radius can't be larger than half of the shortest side
	float4 cornerRadii = clamp(material.ubo.cornerRadii, 0.0, min(halfSize.x, halfSize.y));

	// The face pushed down by press, into the thickness: its bottom doesn't move, the thickness left under the face is depth - press
	float2 faceCenter = float2(0.0, -material.ubo.press);
	float  distance = RoundedRectDistance(input.local - faceCenter, halfSize, cornerRadii);
	float pixelSize = max(0.5 * (length(ddx(input.local)) + length(ddy(input.local))), 1e-5); // units per pixel

	// The border is inside the outline: the fill only starts borderWidth away from it
	float  outer = Coverage(distance, pixelSize);
	float  inner = (material.ubo.borderWidth > 0.0) ? Coverage(distance + material.ubo.borderWidth, pixelSize) : 1.0;
	float4 fill = float4(material.ubo.fillColor.rgb * material.ubo.fillColor.a, material.ubo.fillColor.a);
	float4 border = float4(material.ubo.borderColor.rgb * material.ubo.borderColor.a, material.ubo.borderColor.a);
	float4 shape = lerp(border, fill, inner) * outer; // premultiplied

	// The thickness: the shape swept down by depth, seen under it. Swept vertically, its rounded corners stay the same, only its sides
	// get longer. That body, not only the face, is what casts the shadow. Pressed, it goes from the face's top, moved down, to its
	// bottom, which stays: its shadow shrinks on the top side only, where the body is, and stays in place under it.
	// The body holds the face: what it adds to a pixel is its coverage less the face's, not its coverage over the rest of the pixel.
	// Along the outline they share (the top), that would count the same part of the pixel twice: a fringe of the thickness's color
	float  thickness = material.ubo.depth - material.ubo.press;
	float2 bodyCenter = float2(0.0, -material.ubo.press - 0.5 * thickness);
	float2 bodyHalfSize = halfSize + float2(0.0, 0.5 * thickness);
	if (thickness > 0.0)
	{
		float  bodyCoverage = Coverage(RoundedRectDistance(input.local - bodyCenter, bodyHalfSize, cornerRadii), pixelSize);
		float4 depthColor = float4(material.ubo.depthColor.rgb * material.ubo.depthColor.a, material.ubo.depthColor.a);
		shape += depthColor * max(bodyCoverage - shape.a, 0.0);
	}

	// A drop shadow: the body grown by the spread (its rounded corners too, as a CSS box-shadow), moved by the offset and blurred
	// (shadowBlur = 2 sigma, the CSS convention). On an axis with a direction, the blur only goes that way: the shadow fades away
	// from the shape and never shows on its other side. On an axis without, the blur goes both ways. Never less than about a pixel,
	// which antialiases a sharp shadow
	float4 shadow = float4(0.0, 0.0, 0.0, 0.0);
	if (material.ubo.shadowColor.a > 0.0)
	{
		float2 sigma = max(0.5 * material.ubo.shadowBlur, 0.4 * pixelSize);
		float2 direction = material.ubo.shadowDirection;

		// On an axis the shadow is neither offset nor spread along, its edges fall on the body's. Where they are sharp too (the side
		// opposite to its direction, or no blur), the antialiasing of both would add up into a dark fringe. Pulled in by 1.5 pixels
		// there, under the body, those edges are hidden; where the shadow shows past the body, it is blurred or a pixel narrower
		bool2  coincident = abs(material.ubo.shadowOffset) < pixelSize && abs(material.ubo.shadowSpread) < pixelSize;
		bool2  sharp = direction != 0.0 || sigma <= pixelSize;
		float2 inset = select(coincident && sharp, 1.5 * pixelSize, 0.0);

		float2 shadowHalfSize = max(bodyHalfSize + material.ubo.shadowSpread - inset, 0.0);
		float4 shadowRadii = select(cornerRadii > 0.0, max(cornerRadii + material.ubo.shadowSpread - max(inset.x, inset.y), 0.0), 0.0);
		shadowRadii = min(shadowRadii, min(shadowHalfSize.x, shadowHalfSize.y));
		float2 shadowPoint = input.local - bodyCenter - material.ubo.shadowOffset;
		float  shadowCoverage;
		if (sigma.y <= sigma.x)
		{
			shadowCoverage = BlurredRoundedRect(shadowPoint, shadowHalfSize, shadowRadii, sigma, direction);
		}
		else
		{
			// Mirrored across the diagonal, x and y swapped: the corner (x, y) becomes (y, x), the top left one the bottom right one
			shadowCoverage = BlurredRoundedRect(shadowPoint.yx, shadowHalfSize.yx, shadowRadii.zyxw, sigma.yx, direction.yx);
		}
		shadow = float4(material.ubo.shadowColor.rgb, 1.0) * (material.ubo.shadowColor.a * shadowCoverage);
	}

	// The shape over its shadow, back to straight alpha for the Alpha blend mode
	float4 color = shape + shadow * (1.0 - shape.a);
	if (color.a <= 0.0)
	{
		return float4(0.0, 0.0, 0.0, 0.0);
	}
	return float4(color.rgb / color.a, color.a);
}
