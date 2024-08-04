/*
 state.hpp
 Copyright (c) 2023 DEVTAG. Todos os direitos reservados.
  
 Este código faz parte do software SLAMduino, um produto desenvolvido
 pela DEVTAG. Todos os direitos reservados. A reprodução, distribuição,
 modificação ou uso deste software sem a devida autorização por escrito
 da DEVTAG é estritamente proibida.
  
 A DEVTAG não se responsabiliza por qualquer dano ou prejuízo causado
 pelo uso indevido deste software. Utilize-o por sua conta e risco.
  
 Para obter mais informações, entre em contato com a DEVTAG em:
 davi@devtag.com.br
 https://devtag.com.br
 */

#ifndef STATE_H
#define STATE_H

#include <vector>
#include <string>

#include <fstream>
#include <iostream>

namespace ia {
	namespace rl {
		/// Descreve um estado do ambiente.
		/// @see ia:rl::Env
		class State {
		public:
			virtual ~State() { }

			/// Obtém um vetor com todas as variáveis do ambiente.
			/// @return Vetor com todas as variáveis do ambiente.
			virtual std::vector<double> to_vec() const = 0;
			
			/// Compara dois estados.
			bool operator ==(const State &other) const {
				return to_vec() == other.to_vec();
			}

			/// Formata o estado do ambiente para exibição.
			friend std::ostream& operator<<(std::ostream& os, const State& s) {
				std::vector<double> vars = s.to_vec();
				for (int i=0;i<vars.size()-1;i++)
					os << vars[i] << ",";
				os << vars[vars.size()-1];
				return os;
			}
		};
	}
}

#endif