#include "Vertices.h"


void VertexPos::EnableAttributes(void)
{
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexPos), 0);
}
void VertexPosColor::EnableAttributes(void)
{
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexPosColor), 0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VertexPosColor), (GLvoid*)12);
}
void VertexPosTex1::EnableAttributes(void)
{
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexPosTex1), 0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(VertexPosTex1), (GLvoid*)12);
}
void VertexPosTex2::EnableAttributes(void)
{
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexPosTex2), 0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(VertexPosTex2), (GLvoid*)12);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(VertexPosTex2), (GLvoid*)20);
}
void VertexPosTex1Normal::EnableAttributes(void)
{
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexPosTex1Normal), 0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(VertexPosTex1Normal), (GLvoid*)12);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_TRUE, sizeof(VertexPosTex1Normal), (GLvoid*)20);
}


void VertexPos::DisableAttributes(void)
{
	glDisableVertexAttribArray(0);
}
void VertexPosColor::DisableAttributes(void)
{
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
}
void VertexPosTex1::DisableAttributes(void)
{
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
}
void VertexPosTex2::DisableAttributes(void)
{
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
}
void VertexPosTex1Normal::DisableAttributes(void)
{
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
}