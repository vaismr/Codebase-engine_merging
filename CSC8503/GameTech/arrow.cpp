#include "../GameTech/arrow.h"

using namespace NCL;
using namespace CSC8503;
Arrow::Arrow(Vector3 sp, float l, float w,Vector4 c) {
	this->startPos = sp;
	this->length = l;
	this->width = w;
	this->color = c;

}
void Arrow::Drawarrow() {
	//glBegin(GL_LINES);


}