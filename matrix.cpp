// Matrix.cpp

#include "matrix.h"
#include <iomanip>
#include <cmath>
#include <cstring>

#define DEBUG(s) ;

namespace mtrx {

  /////////////////
 // MATRIX_BASE //
/////////////////

const_matrix_row matrix_base::operator[] (unsigned int r) const { return const_matrix_row(this,r); }

  ////////////
 // MATRIX //
////////////

// Acesso a elementos de matrix
double matrix::at(unsigned int r, unsigned int c) const {
//	if (r>=h || c>=w) throw matrix_error("Erro ao ler elemento: Índices endereçam posição da matriz.");
	return data[r][c];
}

double &matrix::at(unsigned int r, unsigned int c) {
//	if (r>=h || c>=w) throw matrix_error("Erro ao ler elemento: Índices endereçam posição da matriz.");
	return data[r][c];
}

matrix_row matrix::operator[] (int n) { return matrix_row(this, n); }

// Busca por dependências
bool matrix::needs(const matrix_base *p) const {
	return p==this;
}

// Inverter matriz
void matrix::invert() {
DEBUG(std::cerr<<"[Invertendo... "<<std::flush);
	if (w!=h) throw matrix_error("Erro ao inverter matrizes: Matriz não-quadrada.");
	matrix r;
	r=identity_matrix(w);
        for (int i=0; i<h; ++i) {
                double d=data[i][i];
                if (d==0) throw matrix_error("Erro ao inverter matrizes: Matriz singular.");
                for (int j=0; j<w; ++j) {
                        data[i][j]/=d;
                        r.data[i][j]/=d;
                }
                for (int i2=0; i2<h; ++i2) {
                        d=-data[i2][i];
                        if (i!=i2) for (int j=0; j<w; ++j) {
                                data[i2][j]+=d*data[i][j];
                                r.data[i2][j]+=d*r.data[i][j];
                        }
                }
        }
        swap(r);
DEBUG(std::cerr<<" Pronto] "<<std::endl<<std::flush);
}

// Redimensionar uma matriz (perda completa dos dados, valores indeterminados)
void matrix::resize(unsigned int nh, unsigned int nw) {
DEBUG(std::cerr<<"[Redimensionando... "<<std::flush);
	if (w==nw && h==nh) return; // Não precisa redimensionar
	double **odata = data;
	int ow=w, oh=h;
	data=0; // Marca como limpa
	w=h=0;
	if (nw && nh) { // Só aloca se dimensões diferentes de zero
DEBUG(std::cerr<<"Alocando... "<<std::flush);
		data = new double * [nh];
		if (data) for (int i=0; i<nh; ++i) {
			data[i] = new double [nw];
			if (!data[i]) {
				while (i) delete [] data[--i];
				break;
				delete [] data;
				data=0;
			}
			std::memset(data[i], 0, nw*sizeof(double));
		}
		if (!data) throw matrix_error("Erro ao redimensionar matriz: Sem memória disponível.");
		w=nw;
		h=nh;
	}

	if (odata) {
		// Copia dados antigos
DEBUG(std::cerr<<"Copiando... "<<std::flush);
		if (data) for (size_t i=0; i<std::min(oh,h); ++i) {
			for (size_t j=0; j<std::min(ow,w); ++j) {
				data[i][j] = odata[i][j];
			}
		}
		
		// Libera memória anterior
DEBUG(std::cerr<<"Liberando... "<<std::flush);
		for (int i=0; i<oh; ++i) delete [] odata[i];
		delete [] odata;
	}
DEBUG(std::cerr<<" Pronto] "<<std::endl<<std::flush);
}

// Atribuir a partir de uma matriz base
const matrix &matrix::operator=(const matrix_base &m) {
DEBUG(std::cerr<<"[=MTRX] "<<std::flush);
	if (m.needs(this)) { // Evita problemas em operação com auto-dependência
		matrix temp;
		temp = m;
		swap(temp);
	} else {
		resize(m.width(), m.height());
		for (int i=0; i<h; ++i) {
			for (int j=0; j<w; ++j) data[i][j] = m.at(i,j);
		}
	}
	return *this;
}

// Atribuir a partir de uma matriz negada
const matrix &matrix::operator=(const negated_matrix &m) {
DEBUG(std::cerr<<"[=NEG] "<<std::flush);
	const matrix *p=dynamic_cast<const matrix *>(m.A);
	if (this==p) {
		for (int i=0; i<h; ++i) {
			for (int j=0; j<w; ++j) data[i][j]*=-1;
		}
	} else *this = (matrix_base*) &m;
}

// Atribuir a partir de uma matriz invertida
const matrix &matrix::operator=(const inverted_matrix &m) {
DEBUG(std::cerr<<"[=INV "<<std::flush);
	const matrix *p=dynamic_cast<const matrix *>(m.A);
	if (!m.inverted && this==p) {
DEBUG(std::cerr<<"true] "<<std::flush);
		invert();
	} else {
		*this = (matrix_base*) &m;
DEBUG(std::cerr<<"false] "<<std::flush);
	}
}

void matrix::fill(double d) {
	for (int i=0; i<h; ++i) {
		for (int j=0; j<w; ++j) data[i][j]=d;
	}
}

void matrix::multiply(double d) {
	for (int i=0; i<h; ++i) {
		for (int j=0; j<w; ++j) data[i][j]*=d;
	}
}

  ///////////////////////
 // MULTIPLIED_MATRIX //
///////////////////////

// Acesso a elementos de matrix
double multiplied_matrix::at(unsigned int r, unsigned int c) const {
	int nsums=A->width();
	double d=0;
	for (int k=0; k<nsums; ++k) d+=A->at(r,k)*B->at(k,c);
	return d;
}

// Busca por dependências
bool multiplied_matrix::needs(const matrix_base *p) const {
	return A->needs(p) || B->needs(p);
}

  //////////////////
 // ADDED_MATRIX //
//////////////////

// Acesso a elementos de matrix
double added_matrix::at(unsigned int r, unsigned int c) const {
	return A->at(r,c)+B->at(r,c);
}

// Busca por dependências
bool added_matrix::needs(const matrix_base *p) const {
	return A->needs(p) || B->needs(p);
}

  ///////////////////////
 // SUBTRACTED_MATRIX //
///////////////////////

// Acesso a elementos de matrix
double subtracted_matrix::at(unsigned int r, unsigned int c) const {
	return A->at(r,c)-B->at(r,c);
}

// Busca por dependências
bool subtracted_matrix::needs(const matrix_base *p) const {
	return A->needs(p) || B->needs(p);
}

  ////////////////////
 // NEGATED_MATRIX //
////////////////////

// Acesso a elementos de matrix
double negated_matrix::at(unsigned int r, unsigned int c) const {
	return -(A->at(r,c));
}

// Busca por dependências
bool negated_matrix::needs(const matrix_base *p) const {
	return A->needs(p);
}

  /////////////////////
 // INVERTED_MATRIX //
/////////////////////

// Acesso a elementos de matrix
double inverted_matrix::at(unsigned int r, unsigned int c) const {
	if (!inverted) {
		p = new matrix(w,h);
		*p = *A;
		p->invert();
		inverted=true;
	}
	return p->data[r][c];
}

// Busca por dependências
bool inverted_matrix::needs(const matrix_base *p) const {
	return !inverted && A->needs(p);
}

  /////////////////////
 // IDENTITY_MATRIX //
/////////////////////

// Acesso a elementos de matrix
double identity_matrix::at(unsigned int r, unsigned int c) const {
	return r==c;
}

// Busca por dependências
bool identity_matrix::needs(const matrix_base *p) const {
	return false;
}

  ///////////////////////
 // TRANSPOSED_MATRIX //
///////////////////////

// Acesso a elementos de matrix
double transposed_matrix::at(unsigned int r, unsigned int c) const {
	return A->at(c,r);
}

// Busca por dependências
bool transposed_matrix::needs(const matrix_base *p) const {
	return A->needs(p);
}

  ////////////////////////////////
 // Operadores Sobrecarregados //
////////////////////////////////

std::ostream &operator<<(std::ostream &out, const matrix_base &m) {
DEBUG(std::cerr<<"[Imprimindo na tela... "<<std::flush);
	using namespace std;
	for (int i=0; i<m.height(); ++i) {
		out<<"[ ";
		for (int j=0; j<m.width()-1; ++j) out<<setw(14)<<setprecision(4)<<m[i][j]<<", ";
		out<<setw(14)<<setprecision(4)<<m[i][m.width()-1]<<" ]"<<endl;
	}
DEBUG(std::cerr<<"Pronto]"<<std::endl<<std::flush);
	return out;
}

}

