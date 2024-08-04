/*
 envoutcome.hpp
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

#ifndef ENVOUTCOME_H
#define ENVOUTCOME_H

#include "action.hpp"
#include "state.hpp"

#include <iostream>

namespace ia {
	namespace rl {
		/// Estado do ambiente
		class EnvOutcome {
		public:
			State *m_o; ///< Estado atual.
			Action *m_a; ///< Ação executada.
			State *m_op; ///< Próximo estado.
			double m_r; ///< Recompensa recebida.

			bool m_terminated; ///< Fim do episódio.

			/// Cria um EnvOutcome.
			EnvOutcome(State *o, Action *a, State *op, double r, bool terminated) :
				m_o(o), m_a(a), m_op(op), m_r(r), m_terminated(terminated) { }

            /// Cria um EnvOutcome a partir de outro EnvOutcome.
            ///
            /// Este construtor cria uma cópia completa de um outro EnvOutcome
            /// incluindo as referências dos membros **m_o**, **m_a** e **m_op**.
			EnvOutcome(const EnvOutcome &eo) :
				m_o(eo.m_o), m_a(eo.m_a), m_op(eo.m_op), m_r(eo.m_r), m_terminated(eo.m_terminated) { }
			
			virtual ~EnvOutcome() { }

			/// Formata a saída do ambiente para exibição.
			friend std::ostream& operator<<(std::ostream& os, const EnvOutcome& eo) {
				if (eo.m_terminated)
					return os << *eo.m_o << ";" << *eo.m_a << ";" << eo.m_r << ";" << eo.m_terminated;
				else return os << *eo.m_o << ";" << *eo.m_a << ";" << eo.m_r << ";" << *eo.m_op << ";" << eo.m_terminated;
			}
		};
	}
}

#endif
