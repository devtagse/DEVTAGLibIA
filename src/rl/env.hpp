/*
 env.hpp
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

#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include "action.hpp"
#include "state.hpp"
#include "envoutcome.hpp"

#include <vector>

namespace ia {
	namespace rl {
		/// Estrutura de um ambiente de aprendizagem por reforço.
		class Env {
		public:
			virtual ~Env() { }
			
			/// Obtém o estado atual do ambiente.
			/// @return Estado do ambiente.
			virtual State *curr_obs() = 0;

			/// Obtém todas as ações disponíveis.
			/// @return Vetor com todas as ações disponíveis.
			virtual std::vector<Action *> actions() = 0;

			/// Obtém todas as ações realizáveis em um estado do ambiente.
			/// @param state Estado atual do ambiente.
			/// @return Vetor com todas as ações realizáveis no estado do ambiente.
			/// @see ia::rl::Action::is_applicable()
			std::vector<Action *> applicable_actions(State *state) {
				std::vector<Action *> acts;
				for (Action *a : actions()) {
					if (a->is_aplicable(state))
						acts.push_back(a);
				}
				return acts;
			}

			/// Executa uma ação no ambiente.
			/// @param a Ação para executar no ambiente.
			/// @return Estrutura EnvOutcome como resultado da ação realizada no ambiente.
			virtual EnvOutcome *exec_act(Action *a) = 0;
			
			/// Última recompensa recebida.
			/// @return Última recompensa recebida.
			virtual double last_reward() = 0;
			
			/// Verifica se o ambiente está em um estado terminal.
			/// @return **True** se o agente está em um estado terminal ou **false**, caso contrário.
			virtual bool is_terminal() = 0;

			/// Reinicia o ambiente em um estado inicial.
			virtual void reset_env() = 0;
		};
	}
}

#endif
