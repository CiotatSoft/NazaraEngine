// Copyright (C) 2022 Jérôme "Lynix" Leclercq (lynix680@gmail.com)
// This file is part of the "Nazara Engine - Core module"
// For conditions of distribution and use, see copyright notice in Config.hpp

#include <Nazara/Core/Color.hpp>
#include <algorithm>
#include <cmath>
#include <sstream>
#include <Nazara/Core/Debug.hpp>

namespace Nz
{
	/*!
	* \ingroup core
	* \class Color
	* \brief Core class that represents a color
	*/

	/*!
	* \brief Constructs a Color object by default
	*/
	inline Color::Color()
	{
	}

	/*!
	* \brief Constructs a Color object with values
	*
	* \param red Red value
	* \param green Green value
	* \param blue Blue value
	* \param alpha Alpha value
	*/

	inline Color::Color(float red, float green, float blue, float alpha) :
	r(red),
	g(green),
	b(blue),
	a(alpha)
	{
	}

	/*!
	* \brief Constructs a Color object with a light level
	*
	* \param lightness Value for r, g and b
	*/

	inline Color::Color(float lightness) :
	r(lightness),
	g(lightness),
	b(lightness),
	a(1.f)
	{
	}

	/*!
	* \brief Return true is the color has no degree of transparency
	* \return true if the color has an alpha value of 255
	*/
	inline bool Color::IsOpaque() const
	{
		return a >= 1.f;
	}

	/*!
	* \brief Converts this to string
	* \return String representation of the object "Color(r, g, b[, a])"
	*/

	inline std::string Color::ToString() const
	{
		std::ostringstream ss;
		ss << "Color(" << r << ", " << g << ", " << b;

		if (!IsOpaque())
			ss << ", " << a;

		ss << ')';

		return ss.str();
	}

	/*!
	* \brief Adds two colors together
	* \return Color which is the sum
	*
	* \param color Other color
	*/

	inline Color Color::operator+(const Color& color) const
	{
		Color c;
		c.r = r + color.r;
		c.g = g + color.g;
		c.b = b + color.b;
		c.a = a + color.a;

		return c;
	}

	/*!
	* \brief Multiplies two colors together
	* \return Color which is the product
	*
	* \param color Other color
	*/

	inline Color Color::operator*(const Color& color) const
	{
		Color c;
		c.r = r * color.r;
		c.g = g * color.g;
		c.b = b * color.b;
		c.a = a * color.a;

		return c;
	}

	/*!
	* \brief Adds the color to this
	* \return Color which is the sum
	*
	* \param color Other color
	*/

	inline Color Color::operator+=(const Color& color)
	{
		return operator=(operator+(color));
	}

	/*!
	* \brief Multiplies the color to this
	* \return Color which is the product
	*
	* \param color Other color
	*/

	inline Color Color::operator*=(const Color& color)
	{
		return operator=(operator*(color));
	}

	/*!
	* \brief Checks whether the two colors are equal
	* \return true if it is the case
	*
	* \param color Color to compare
	*/

	inline bool Color::operator==(const Color& color) const
	{
		return NumberEquals(r, color.r) &&
		       NumberEquals(g, color.g) &&
		       NumberEquals(b, color.b) &&
		       NumberEquals(a, color.a);
	}

	/*!
	* \brief Checks whether the two colors are equal
	* \return false if it is the case
	*
	* \param color Color to compare
	*/

	inline bool Color::operator!=(const Color& color) const
	{
		return !operator==(color);
	}

	// Algorithm coming from http://www.easyrgb.com/index.php?X=MATH

	/*!
	* \brief Converts CMY representation to RGB
	* \return Color resulting
	*
	* \param cyan Cyan component
	* \param magenta Magenta component
	* \param yellow Yellow component
	*/
	inline Color Color::FromCMY(float cyan, float magenta, float yellow)
	{
		return Color(1.f - cyan, 1.f - magenta, 1.f - yellow);
	}

	/*!
	* \brief Converts CMYK representation to RGB
	* \return Color resulting
	*
	* \param cyan Cyan component
	* \param magenta Magenta component
	* \param yellow Yellow component
	* \param black Black component
	*/
	inline Color Color::FromCMYK(float cyan, float magenta, float yellow, float black)
	{
		return FromCMY(cyan * (1.f - black) + black,
		               magenta * (1.f - black) + black,
		               yellow * (1.f - black) + black);
	}

	/*!
	* \brief Converts HSL representation to RGB
	* \return Color resulting
	*
	* \param hue Hue component in [0, 360]
	* \param saturation Saturation component [0, 1]
	* \param lightness Lightness component [0, 1]
	*/
	inline Color Color::FromHSL(float hue, float saturation, float lightness)
	{
		if (NumberEquals(saturation, 0.f))
			return Color(lightness);
		else
		{
			float v2;
			if (lightness < 0.5f)
				v2 = lightness * (1.f + saturation);
			else
				v2 = (lightness + saturation) - (saturation * lightness);

			float v1 = 2.f * lightness - v2;

			float h = hue / 360.f;
			return Color(Hue2RGB(v1, v2, h + (1.f/3.f)),
			             Hue2RGB(v1, v2, h),
			             Hue2RGB(v1, v2, h - (1.f/3.f)));
		}
	}

	/*!
	* \brief Converts HSV representation to RGB
	* \return Color resulting
	*
	* \param hue Hue component in [0, 360]
	* \param saturation Saturation component in [0, 1]
	* \param value Value component in [0, 1]
	*/
	inline Color Color::FromHSV(float hue, float saturation, float value)
	{
		if (NumberEquals(saturation, 0.f))
			return Color(value);
		else
		{
			float h = (hue / 360.f) * 6.f;

			if (NumberEquals(h , 6.f))
				h = 0.f; // hue must be < 1

			int i = static_cast<int>(h);
			float v1 = value * (1.f - saturation);
			float v2 = value * (1.f - saturation * (h - i));
			float v3 = value * (1.f - saturation * (1.f - (h - i)));

			float r, g, b;
			switch (i)
			{
				case 0:
					r = value;
					g = v3;
					b = v1;
					break;

				case 1:
					r = v2;
					g = value;
					b = v1;
					break;

				case 2:
					r = v1;
					g = value;
					b = v3;
					break;

				case 3:
					r = v1;
					g = v2;
					b = value;
					break;

				case 4:
					r = v3;
					g = v1;
					b = value;
					break;

				default:
					r = value;
					g = v1;
					b = v2;
					break;
			}

			return Color(r, g, b);
		}
	}

	/*!
	* \brief Converts RGB8 representation to Color (RGBA32F)
	* \return Color resulting
	*
	* \param r Red value
	* \param g Green value
	* \param b Blue value
	*/
	inline Color Color::FromRGB8(UInt8 r, UInt8 g, UInt8 b)
	{
		return Color(r / 255.f, g / 255.f, b / 255.f);
	}
	
	/*!
	* \brief Converts RGBA8 representation to Color (RGBA32F)
	* \return Color resulting
	*
	* \param r Red value
	* \param g Green value
	* \param b Blue value
	* \param a Alpha value
	*/
	inline Color Color::FromRGBA8(UInt8 r, UInt8 g, UInt8 b, UInt8 a)
	{
		return Color(r / 255.f, g / 255.f, b / 255.f, a / 255.f);
	}

	/*!
	* \brief Converts XYZ representation to RGB
	* \return Color resulting
	*
	* \param vec Vector3 representing the space color
	*/
	inline Color Color::FromXYZ(const Vector3f& vec)
	{
		return FromXYZ(vec.x, vec.y, vec.z);
	}

	/*!
	* \brief Converts XYZ representation (D65/2°) to RGB
	* \return Color resulting
	*
	* \param x X component
	* \param y Y component
	* \param z Z component
	*/
	inline Color Color::FromXYZ(float x, float y, float z)
	{
		x /= 100.f; // X from 0 to  95.047
		y /= 100.f; // Y from 0 to 100.000
		z /= 100.f; // Z from 0 to 108.883

		float r = x *  3.2406f + y * -1.5372f + z * -0.4986f;
		float g = x * -0.9689f + y *  1.8758f + z *  0.0415f;
		float b = x *  0.0557f + y * -0.2040f + z *  1.0570f;

		if (r > 0.0031308f)
			r = 1.055f * (std::pow(r, 1.f/2.4f)) - 0.055f;
		else
			r *= 12.92f;

		if (g > 0.0031308f)
			g = 1.055f * (std::pow(g, 1.f/2.4f)) - 0.055f;
		else
			g *= 12.92f;

		if (b > 0.0031308f)
			b = 1.055f * (std::pow(b, 1.f/2.4f)) - 0.055f;
		else
			b *= 12.92f;

		return Color(r, g, b);
	}

	/*!
	* \brief Converts RGB representation to CMYK
	*
	* \param color Color to transform
	* \param cyan Cyan component
	* \param magenta Magenta component
	* \param yellow Yellow component
	*/
	inline void Color::ToCMY(const Color& color, float* cyan, float* magenta, float* yellow)
	{
		*cyan = 1.f - color.r;
		*magenta = 1.f - color.g;
		*yellow = 1.f - color.b;
	}

	/*!
	* \brief Converts RGB representation to CMYK
	*
	* \param color Color to transform
	* \param cyan Cyan component
	* \param magenta Magenta component
	* \param yellow Yellow component
	*/

	inline void Color::ToCMYK(const Color& color, float* cyan, float* magenta, float* yellow, float* black)
	{
		float c, m, y;
		ToCMY(color, &c, &m, &y);

		float k = std::min({1.f, c, m, y});

		if (NumberEquals(k, 1.f))
		{
			//Black
			*cyan = 0.f;
			*magenta = 0.f;
			*yellow = 0.f;
		}
		else
		{
			*cyan = (c-k)/(1.f-k);
			*magenta = (m-k)/(1.f-k);
			*yellow = (y-k)/(1.f-k);
		}

		*black = k;
	}

	/*!
	* \brief Converts RGB representation to HSL
	*
	* \param color Color to transform
	* \param hue Hue component [0, 360]
	* \param saturation Saturation component in [0, 1]
	* \param lightness Lightness component in [0, 1]
	*/
	inline void Color::ToHSL(const Color& color, float* hue, float* saturation, float* lightness)
	{
		float r = color.r;
		float g = color.g;
		float b = color.b;

		float min = std::min({r, g, b}); // Min. value of RGB
		float max = std::max({r, g, b}); // Max. value of RGB

		float deltaMax = max - min; //Delta RGB value

		float l = (max + min) / 2.f;
		*lightness = l;

		if (NumberEquals(deltaMax, 0.f))
		{
			//This is a gray, no chroma...
			*hue = 0.f;
			*saturation = 0.f;
		}
		else
		{
			if (l <= 0.5f)
				*saturation = deltaMax / (max + min);
			else
				*saturation = (deltaMax / (2.f - max - min));

			float deltaR = ((max - r) / 6.f + deltaMax / 2.f) / deltaMax;
			float deltaG = ((max - g) / 6.f + deltaMax / 2.f) / deltaMax;
			float deltaB = ((max - b) / 6.f + deltaMax / 2.f) / deltaMax;

			float h;

			if (NumberEquals(r, max))
				h = deltaB - deltaG;
			else if (NumberEquals(g, max))
				h = (1.f / 3.f) + deltaR - deltaB;
			else
				h = (2.f / 3.f) + deltaG - deltaR;

			if (h < 0.f)
				h += 1.f;
			else if (h > 1.f)
				h -= 1.f;

			*hue = h * 360.f;
		}
	}

	/*!
	* \brief Converts RGB representation to HSV
	*
	* \param color Color to transform
	* \param hue Hue component
	* \param saturation Saturation component
	* \param value Value component
	*/
	inline void Color::ToHSV(const Color& color, float* hue, float* saturation, float* value)
	{
		float r = color.r;
		float g = color.g;
		float b = color.b;

		float min = std::min({r, g, b});    //Min. value of RGB
		float max = std::max({r, g, b});    //Max. value of RGB

		float deltaMax = max - min; //Delta RGB value

		*value = max;

		if (NumberEquals(deltaMax, 0.f))
		{
			//This is a gray, no chroma...
			*hue = 0.f;
			*saturation = 0.f;
		}
		else
		{
			//Chromatic data...
			*saturation = deltaMax / max;

			float deltaR = ((max - r) / 6.f + deltaMax / 2.f) / deltaMax;
			float deltaG = ((max - g) / 6.f + deltaMax / 2.f) / deltaMax;
			float deltaB = ((max - b) / 6.f + deltaMax / 2.f) / deltaMax;

			float h;

			if (NumberEquals(r, max))
				h = deltaB - deltaG;
			else if (NumberEquals(g, max))
				h = (1.f / 3.f) + deltaR - deltaB;
			else
				h = (2.f / 3.f) + deltaG - deltaR;

			if (h < 0.f)
				h += 1.f;
			else if (h > 1.f)
				h -= 1.f;

			*hue = h * 360.f;
		}
	}

	/*!
	* \brief Converts Color representation (RGBA32F) to RGB8
	*
	* \param color Color to transform
	* \param r Red component
	* \param g Green component
	* \param b Blue component
	*
	* \remark Values are clamped to [0;255]
	*/
	static inline void ToRGB8(const Color& color, UInt8* r, UInt8* g, UInt8* b)
	{
		*r = static_cast<UInt8>(Clamp(color.r * 255.f, 0.f, 255.f));
		*g = static_cast<UInt8>(Clamp(color.g * 255.f, 0.f, 255.f));
		*b = static_cast<UInt8>(Clamp(color.b * 255.f, 0.f, 255.f));
	}

	/*!
	* \brief Converts Color representation (RGBA32F) to RGBA8
	*
	* \param color Color to transform
	* \param r Red component
	* \param g Green component
	* \param b Blue component
	* \param a Alpha component
	*
	* \remark Values are clamped to [0;255]
	*/
	static inline void ToRGBA8(const Color& color, UInt8* r, UInt8* g, UInt8* b, UInt8* a)
	{
		*r = static_cast<UInt8>(Clamp(color.r * 255.f, 0.f, 255.f));
		*g = static_cast<UInt8>(Clamp(color.g * 255.f, 0.f, 255.f));
		*b = static_cast<UInt8>(Clamp(color.b * 255.f, 0.f, 255.f));
		*a = static_cast<UInt8>(Clamp(color.a * 255.f, 0.f, 255.f));
	}

	/*!
	* \brief Converts RGB representation to XYZ
	*
	* \param color Color to transform
	* \param vec Vector3 representing the space color
	*/
	inline void Color::ToXYZ(const Color& color, Vector3f* vec)
	{
		return ToXYZ(color, &vec->x, &vec->y, &vec->z);
	}

	/*!
	* \brief Converts RGB representation to XYZ
	*
	* \param color Color to transform
	* \param x X component
	* \param y Y component
	* \param z Z component
	*/
	inline void Color::ToXYZ(const Color& color, float* x, float* y, float* z)
	{
		float r = color.r; //< R from 0 to 255
		float g = color.g; //< G from 0 to 255
		float b = color.b; //< B from 0 to 255

		if (r > 0.04045f)
			r = std::pow((r + 0.055f)/1.055f, 2.4f);
		else
			r /= 12.92f;

		if (g > 0.04045f)
			g = std::pow((g + 0.055f)/1.055f, 2.4f);
		else
			g /= 12.92f;

		if (b > 0.04045f)
			b = std::pow((b + 0.055f)/1.055f, 2.4f);
		else
			b /= 12.92f;

		r *= 100.f;
		g *= 100.f;
		b *= 100.f;

		//Observer. = 2°, Illuminant = D65
		*x = r*0.4124f + g*0.3576f + b*0.1805f;
		*y = r*0.2126f + g*0.7152f + b*0.0722f;
		*z = r*0.0193f + g*0.1192f + b*0.9505f;
	}

	/*!
	* \brief Converts HUE representation to RGV
	* \return RGB corresponding
	*
	* \param v1 V1 component
	* \param v2 V2 component
	* \param vH VH component
	*/
	inline float Color::Hue2RGB(float v1, float v2, float vH)
	{
		if (vH < 0.f)
			vH += 1;

		if (vH > 1.f)
			vH -= 1;

		if ((6.f * vH) < 1.f)
			return v1 + (v2-v1)*6*vH;

		if ((2.f * vH) < 1.f)
			return v2;

		if ((3.f * vH) < 2.f)
			return v1 + (v2 - v1)*(2.f/3.f - vH)*6;

		return v1;
	}

	/*!
	* \brief Serializes a Color
	* \return true if successfully serialized
	*
	* \param context Serialization context
	* \param color Input color
	*/
	inline bool Serialize(SerializationContext& context, const Color& color, TypeTag<Color>)
	{
		if (!Serialize(context, color.r))
			return false;

		if (!Serialize(context, color.g))
			return false;

		if (!Serialize(context, color.b))
			return false;

		if (!Serialize(context, color.a))
			return false;

		return true;
	}

	/*!
	* \brief Unserializes a Color
	* \return true if successfully unserialized
	*
	* \param context Serialization context
	* \param color Output color
	*/
	inline bool Unserialize(SerializationContext& context, Color* color, TypeTag<Color>)
	{
		if (!Unserialize(context, &color->r))
			return false;

		if (!Unserialize(context, &color->g))
			return false;

		if (!Unserialize(context, &color->b))
			return false;

		if (!Unserialize(context, &color->a))
			return false;

		return true;
	}
}

/*!
* \brief Output operator
* \return The stream
*
* \param out The stream
* \param color The color to output
*/

inline std::ostream& operator<<(std::ostream& out, const Nz::Color& color)
{
	return out << color.ToString();
}

#include <Nazara/Core/DebugOff.hpp>
