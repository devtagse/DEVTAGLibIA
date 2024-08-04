/*
 action.hpp
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

#ifndef ACTION_H
#define ACTION_H

#include "state.hpp"

#include <vector>
#include <string>

namespace ia {
	/// Aprendizagem por reforço.
	namespace rl {
		/// Classe para definir as ações de um agente.
		///
		/// Crie uma nova classe que estende Action e que implemente a função
		/// is_aplicable().
		/// @see robot::motor_control::MotorControlActions
		class Action {
		public:
			std::string m_name; ///< Nome da ação
			int m_num; ///< Número da ação

			/// Constrói uma ação com nome e número.
			/// @param name Nome da ação.
			/// @param num Número da ação.
			Action(std::string name, int num) : m_name(name), m_num(num) { }

			virtual ~Action() { }

			/// Compara duas ações pelo nome.
			bool operator <(const Action& other) const {
				return m_name.compare(other.m_name) < 0;
			}

			/// Compara duas ações pelo nome.
			bool operator ==(const Action& other) const {
				return m_name.compare(other.m_name) == 0;
			}

			/// Verifica se esta ação pode ser executada em um estado.
			/// @param s Estado.
			/// @return **True** se a ação puder ser executada ou **false**, caso contrário.
			/// É preciso criar uma classe que estenda de Action e que implemente is_aplicable().
			/// @see robot::motor_control::MotorControlActions
			virtual bool is_aplicable(State *s) {
				return true;
			}

			/// Nome da ação.
			friend std::ostream& operator<<(std::ostream& os, const Action& a) {
				return os << a.m_name;
			}
		};
	}
}

#endif
