#ifndef _TYPES_CH
#define _TYPES_CH

#include "Vec2.h"

namespace RoninEngine
{
	enum Align
	{
		Left,
		Right,
		Center
	};

	template<typename _Ty>
	struct xVector2
	{
		using type = xVector2<_Ty>;
	public:
		_Ty x;
		_Ty y;

		xVector2()
		{
			x = 0;
			y = 0;
		}

		xVector2(const _Ty& x, const _Ty& y)
		{
			this->x = x;
			this->y = y;
		}

		_Ty sqrMagnitude() const
		{
			return x * x + y * y;
		}

		_Ty magnitude() const
		{
			return sqrtf(x * x + y * y);
		}

		bool is_normalized()
		{
			// todo: check
			return false;
		}

		type normalized()
		{
			//todo: написать нормализацию
			return zero;
		}

		type operator +(const type& rhs)
		{
			type v = *this;
			v.x += rhs.x;
			v.y += rhs.y;
			return v;
		}

		type operator -(const type& rhs)
		{
			type v = *this;
			v.x -= rhs.x;
			v.y -= rhs.y;
			return v;
		}

		type operator *(const _Ty& rhs)
		{
			type v = *this;
			v.x *= rhs;
			v.y *= rhs;
			return v;
		}

		type operator /(const _Ty& rhs)
		{
			type v = *this;
			v.x /= rhs;
			v.y /= rhs;
			return v;
		}

		type& operator +=(const type& rhs)
		{
			x += rhs.x;
			y += rhs.y;
			return *this;
		}

		type& operator -=(const type& rhs)
		{
			x -= rhs.x;
			y -= rhs.y;
			return *this;
		}

		type& operator *=(const _Ty& rhs)
		{
			x *= rhs;
			y *= rhs;
			return *this;
		}

		type& operator /=(const _Ty& rhs)
		{
			x /= rhs;
			y /= rhs;
			return *this;
		}

		bool operator ==(const type& rhs)
		{
			return this->x == rhs.x && this->y == rhs.y;
		}

		bool operator !=(const type& rhs)
		{
			return !(*this == rhs);
		}

		static float distance(const type& lhs, const type& rhs)
		{
			return sqrtf(abs((rhs.x - lhs.x) + (rhs.y - lhs.y)));
		}

		static const type up, right, left, down, zero, one, minusOne;
	};


	template<typename _Ty>
	struct xRect
	{
	public:
		using classX = xRect<_Ty>;
		using const_classX = const xRect<_Ty>&;
		using type_link = _Ty&;
		using const_type_link = const _Ty&;

		_Ty x, y, w, h;

		type_link operator[](int index)
		{
			switch (index)
			{
			case 0:
				return x;
			case 1:
				return y;
			case 2:
				return w;
			case 3:
				return h;
			}
			throw std::exception("param index");
		}

		xRect()
		{
			x = 0;
			y = 0;
			w = 0;
			h = 0;
		}

		xRect(const_type_link copy)
		{
			x = y = w = h = copy;
		}

		xRect(const xVector2<_Ty>& point, const xVector2<_Ty>& size)
		{
			x = static_cast<_Ty>(point.x);
			y = static_cast<_Ty>(point.y);
			w = static_cast<_Ty>(size.x);
			h = static_cast<_Ty>(size.y);
		}

		xRect(const_type_link left, const_type_link top, const_type_link right, const_type_link bottom)
		{
			this->x = left;
			this->y = top;
			this->w = right;
			this->h = bottom;
		}

		xVector2<_Ty> to_Point()
		{
			return xVector2<_Ty>(static_cast<_Ty>(x), static_cast<_Ty>(y));
		}

		xVector2<_Ty> to_Size()
		{
			return xVector2<_Ty>(static_cast<_Ty>(w - x), static_cast<_Ty>(h - y));
		}

		classX& incrementWithSign()
		{
			_Ty v = (_Ty)0x1;
			return addWithSign(v);
		}

		classX& addWithSign(const_type_link vector)
		{
			_Ty sign;
			for (int i = 3; i != ~0; --i)
			{
				sign = *this[i] < 0 ? -1 : *this[i]>0 ? 1 : 0;
				*this[i] += sign * vector;
			}
			return this;
		}
		
		const bool empty() const {
			return false;
			//return (x|y|w|h)==0;
		}

		bool operator ==(classX& rhs)
		{
			int i;
			for (i = 0; i < 4; ++i)
				if (this->operator[](i) != rhs[i])
					return false;
			return true;
		}

		bool operator !=(classX& rhs)
		{
			return !this->operator==(rhs);
		}

		classX operator +(classX rhs)
		{
			auto _R = *this;
			for (int i = 3; i != ~0; --i)
				_R[i] += rhs[i];
			return  _R;
		}

		classX operator -(classX rhs)
		{
			classX _R = *this;
			for (int i = 3; i != ~0; --i)
				_R[i] -= rhs[i];
			return  _R;
		}

		classX& operator +=(classX rhs)
		{
			for (int i = 3; i != ~0; --i)
				(*this)[i] += rhs[i];
			return  *this;
		}

		classX& operator -=(classX rhs)
		{
			for (int i = 3; i != ~0; --i)
				(*this)[i] -= rhs[i];
			return  *this;
		}

		classX operator /(const_type_link value)
		{
			classX lhs = *this;
			for (int i = 3; i != ~0; --i)
				lhs[i] /= value;
			return lhs;
		}

		classX operator *(const_type_link value)
		{
			classX lhs = *this;
			for (int i = 3; i != ~0; --i)
				lhs[i] *= value;
			return lhs;
		}

		friend classX operator +(const_classX lhs, const_classX rhs)
		{
			classX cc = lhs;
			cc += rhs;
			return cc;
		}

		friend classX operator -(const_classX lhs, const_classX rhs)
		{
			classX cc = lhs;
			cc -= rhs;
			return cc;
		}

		template<typename _From, typename _To>
		static xRect<_To> convert_to(const xRect<_From>& value)
		{
			xRect<_To> result;
			for (int i = 3; i != ~0; --i)
				result[i] = static_cast<_To>(value[i]);
			return result;
		}


		static const_classX zero() { return classX(); }
		static const_classX one() { return classX(1); }
		static const_classX minusOne() { return classX(-1); }
	};

	typedef xVector2<int> Vector2i;

	typedef xRect<int> Rect_t;
	typedef xRect<float> Rectf_t;

	double Get_Angle(Vec2 lhs, Vec2 rhs);


}
#endif