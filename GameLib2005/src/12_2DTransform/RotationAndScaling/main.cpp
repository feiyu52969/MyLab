#include "GameLib/GameLib.h"
#include "GameLib/Framework.h"
#include "Image.h"
#include "Vector2.h"
#include "Matrix22.h"
#include "GameLib/Math.h"
#include <sstream>
using namespace std;

int round( double a ){
	a += ( a > 0.0 ) ? 0.5 : -0.5f;
	return static_cast< int >( a );
}

void transform(
Vector2* out,
const Vector2& in,
const Vector2& scalingOffset,
const Vector2& scalingRatio,
const Vector2& rotationOffset,
const Matrix22& rotationMatrix ){
	//�g��k��
	out->setMul( scalingRatio, in );
	//�ړ�
	*out += scalingOffset;
	//��]���S���炵
	*out -= rotationOffset;
	//��]
	rotationMatrix.multiply( out, *out );
	//��]���S���炵���ǂ�
	*out += rotationOffset;
}

bool gFirstFrame = true;
Image* gImage;
int gCount;

namespace GameLib{
	void Framework::update(){
		if ( gFirstFrame ){
			gImage = new Image( "background.dds" );
			gFirstFrame = false;
		}
		unsigned* vram = videoMemory();
		int ww = width(); //window width
		int wh = height(); //window height
		//��U�S���^������
		for ( int i = 0; i < ww * wh; ++i ){
			vram[ i ] = 0;
		}
		int iw = gImage->width(); //image width
		int ih = gImage->height(); //image height
		double rotation = static_cast< double >( gCount ); //������g�嗦�Ɏg����
		//�g�嗦
		Vector2 scalingRatio( 1.1 + sin( rotation ), 1.1 + cos( rotation ) );
		//�g��I�t�Z�b�g(�g�嗦�ɉ����ĕς�邱�Ƃɒ���)
		Vector2 scalingOffset( ww/2 - iw/2*scalingRatio.x, wh/2 - ih/2*scalingRatio.y );
		//��]�I�t�Z�b�g
		Vector2 rotationOffset( ww/2, wh/2 );
		//��]�s��
		double sine = sin( rotation );
		double cosine = cos( rotation );
		Matrix22 rotationMatrix( cosine, -sine, sine, cosine );
		
		//3�_���
		Vector2 a, b, c;
		transform( &a, Vector2( 0, 0 ), scalingOffset, scalingRatio, rotationOffset, rotationMatrix ); //����
		transform( &b, Vector2( iw, 0 ), scalingOffset, scalingRatio, rotationOffset, rotationMatrix ); //�E��
		transform( &c, Vector2( 0, ih ), scalingOffset, scalingRatio, rotationOffset, rotationMatrix ); //����
		//b-a,c-a���v�Z
		Vector2 ab, ac;
		ab.setSub( b, a );
		ac.setSub( c, a );
		//��ԊJ�n
		double rcpWidth = 1.0 / static_cast< double >( iw );
		double rcpHeight = 1.0 / static_cast< double >( ih );
		for ( int y = 0; y < ih; ++y ){
			double yf = static_cast< double >( y ) + 0.5;
			double v = yf * rcpHeight;
			for ( int x = 0; x < iw; ++x ){
				double xf = static_cast< double >( x ) + 0.5;
				double u = xf * rcpWidth;
				Vector2 p;
				p.setInterporation( a, ab, ac, u, v );
				p -= Vector2( 0.5, 0.5 ); //�Y������
				int rx, ry;
				rx = round( p.x );
				ry = round( p.y );
				//�͈͓��Ȃ�\��t��
				if ( rx >= 0 && rx < ww && ry >= 0 && ry < wh ){
					vram[ ry * ww + rx ] = gImage->pixel( x, y );
				}
			}
		}
		++gCount;

		ostringstream oss;
		oss << frameRate();
		drawDebugString( 0, 0, oss.str().c_str() );
	}
}