// matrix.h
#ifndef MATRIX_H
#define MATRIX_H

#include <iostream>

namespace mtrx {

class matrix;
class multiplied_matrix;
class added_matrix;
class negated_matrix;
class identity_matrix;
class inverted_matrix;
class transposed_matrix;
class matrix_row;
class const_matrix_row;

// Objeto para represntação de erros:
class matrix_error {
	const char *str;
	public:
	matrix_error(const char *p) : str(p) {};
	matrix_error() : str("Erro não identificado com matriz.") {}
	operator const char* () const { return str; }
};

// Objeto que representa faz a abstração de uma matriz.
class matrix_base {
	protected:
	int w,h;
	virtual double at(unsigned int r, unsigned int c) const = 0;
	virtual bool needs(const matrix_base *p) const = 0;
	
	public:
	matrix_base() : w(0), h(0) {}
	matrix_base(int nw, int nh) : w(nw), h(nh) {}
	int width() const { return w; }
	int height() const { return h; }
	const_matrix_row operator[] (unsigned int r) const;

	friend class matrix_row;
	friend class const_matrix_row;
	friend class matrix;
	friend class added_matrix;
	friend class subtracted_matrix;
	friend class negated_matrix;
	friend class identity_matrix;
	friend class inverted_matrix;
	friend class multiplied_matrix;
	friend class transposed_matrix;
	friend std::ostream &operator<<(std::ostream &out, const matrix_base &m);
};

// Objeto para representação de uma linha constante. Possibilita A[i][j].
class const_matrix_row {
	protected:
	const matrix_base *p;
	int r;
	public:
	const_matrix_row(const matrix_base *mx, unsigned int row) : p(mx), r(row) {}
	double operator [] (unsigned int c) {
		if (r>=p->height() || c>=p->width()) throw matrix_error("Erro ao ler elemento: Índices endereçam posição da matriz.");
		return p->at(r,c);
	}
};

// Objeto representativo das multiplicações
class multiplied_matrix : public matrix_base {
	const matrix_base *A, *B;
	double at(unsigned int r, unsigned int c) const;
	bool needs(const matrix_base *p) const;
	public:
	multiplied_matrix(const matrix_base *m1, const matrix_base *m2) {
		if (m1->width() != m2->height()) throw matrix_error("Erro ao multiplicar matrizes: Dimensões não aceitáveis.");
		A = m1;
		B = m2;
		h = m1->height();
		w = m2->width();
	}
};

// Objeto representativo das somas
class added_matrix : public matrix_base {
	const matrix_base *A, *B;
	double at(unsigned int r, unsigned int c) const;
	bool needs(const matrix_base *p) const;
	public:
	added_matrix(const matrix_base *m1, const matrix_base *m2) {
		if (m1->width() != m2->width() || m1->height() != m2->height()) throw matrix_error("Erro ao somar matrizes: Dimensões não aceitáveis.");
		A = m1;
		B = m2;
		h = m1->height();
		w = m2->width();
	}
};

// Objeto representativo das subtrações
class subtracted_matrix : public matrix_base {
	const matrix_base *A, *B;
	double at(unsigned int r, unsigned int c) const;
	bool needs(const matrix_base *p) const;
	public:
	subtracted_matrix(const matrix_base *m1, const matrix_base *m2) {
		if (m1->width() != m2->width() || m1->height() != m2->height()) throw matrix_error("Erro ao subtrair matrizes: Dimensões não aceitáveis.");
		A = m1;
		B = m2;
		h = m1->height();
		w = m2->width();
	}
};

// Objeto representativo das inversões de sinal
class negated_matrix : public matrix_base {
	const matrix_base *A;
	double at(unsigned int r, unsigned int c) const;
	bool needs(const matrix_base *p) const;
	public:
	negated_matrix(const matrix_base *m) : A(m), matrix_base(m->height(), m->width()) {}
	friend class matrix;
};

// Objeto representando uma matriz identidade
class identity_matrix : public matrix_base {
	double at(unsigned int r, unsigned int c) const;
	bool needs(const matrix_base *p) const;
	public:
	identity_matrix(unsigned int ord) : matrix_base(ord,ord) {}
};

// Objeto representando uma matriz com dados.
class matrix : public matrix_base {
	double **data;
	double at(unsigned int r, unsigned int c) const;
	double &at(unsigned int r, unsigned int c);
	bool needs(const matrix_base *p) const;
	void invert();
	void swap(matrix &a) {
		unsigned int i = a.w;
		a.w = w;
		w = i;
		i = a.h;
		a.h = h;
		h = i;
		double **p = a.data;
		a.data = data;
		data = p;
	}
	public:
	matrix() : data(0) {}
	~matrix() { resize(0,0); }
	void resize(unsigned int rows, unsigned int cols);
	matrix(int rows, int cols) : data(0) { resize(rows,cols); }
	const matrix &operator=(const matrix_base &m);
	const matrix &operator=(const inverted_matrix &m);
	const matrix &operator=(const negated_matrix &m);
	void fill(double d);
	void multiply(double d);
	void divide(double d) { multiply(1/d); }
	void zero() { fill(0); }
	
	matrix_row operator[] (int n);
	
	friend class inverted_matrix;
	friend class matrix_row;
};

// Objeto para representação de uma linha não-constante. Possibilita A[i][j].
class matrix_row {
	protected:
	matrix *p;
	int r;
	public:
	matrix_row(matrix *mx, unsigned int row) : p(mx), r(row) {}
	double &operator [] (unsigned int c) {
		if (r>=p->height() || c>=p->width()) throw matrix_error("Erro ao ler elemento: Índices endereçam posição da matriz.");
		return p->at(r,c);
	}
};

// Objeto representando o inverso de uma matriz
class inverted_matrix : public matrix_base {
	const matrix_base * A;
	mutable matrix * p;
	mutable bool inverted;
	double at(unsigned int r, unsigned int c) const;
	bool needs(const matrix_base *p) const;
	public:
	inverted_matrix(const matrix_base &m) {
		if (m.width()!=m.height()) throw matrix_error("Erro ao inverter matrizes: Matriz não-quadrada.");
		w=m.w;
		h=m.h;
		A=&m;
		inverted=false;
	}
	~inverted_matrix() { if (inverted) delete p; }

	friend class matrix;
};

// Objeto representando uma matriz transposta
class transposed_matrix : public matrix_base {
	const matrix_base * A;
	double at(unsigned int r, unsigned int c) const;
	bool needs(const matrix_base *p) const;
	public:
	transposed_matrix(const matrix_base &m) : A(&m), matrix_base(m.height(),m.width()) {}
};

// Operadores dobrecarregados:
inline multiplied_matrix operator*(const matrix_base &A, const matrix_base &B) {
	return multiplied_matrix(&A,&B);
}

inline added_matrix operator+(const matrix_base &A, const matrix_base &B) {
	return added_matrix(&A,&B);
}

inline subtracted_matrix operator-(const matrix_base &A, const matrix_base &B) {
	return subtracted_matrix(&A,&B);
}


inline negated_matrix operator- (const matrix_base &A) {
	return negated_matrix(&A);
}

inline inverted_matrix inv(const matrix_base &A) {
	return inverted_matrix(A);
}

inline transposed_matrix transpose(const matrix_base &A) {
	return transposed_matrix(A);
}

std::ostream &operator<<(std::ostream &out, const matrix_base &m);

}

#endif

