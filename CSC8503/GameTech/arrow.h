#pragma once
#include "..//CSC8503Common/RenderObject.h"
#include "..//..//Common/Maths.h"
#include "..//..//Common/Vector3.h"

namespace NCL {
	namespace CSC8503
	{
		class Arrow  {
		public:
			Arrow(Vector3 sp, float l, float w,Vector4 c);
			Vector3 startPos;
			float length;
			float width;
			Vector4 color;
			void Drawarrow();
		};
	}
}