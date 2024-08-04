/*
 episode.hpp
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

#ifndef EPISODE_H
#define EPISODE_H

#include "state.hpp"
#include "action.hpp"
#include "envoutcome.hpp"

namespace ia {
	namespace rl {
		/// Descreve os acontecimentos de um episódio.
		///
		/// Esta classe armazena toda a sequência /f$ (s,a,r) /f$ de um episódio.
		/// @see GDSarsaLambda::run_learning()
		class Episode {
		public:
			std::vector<State *> m_state_seq; ///< Sequência de estados de um episódio.
			std::vector<Action *> m_action_seq; ///< Sequência de ações de um episódio.
			std::vector<double> m_reward_seq; ///< Sequência de recompensas de um episódio.
			
			/// Cria um episódio sem nenhuma sequência de estados, ações ou recompensas.
			Episode();

			/// Cria um episódio com o estado inicial na sequêcia de estados do episódio.
			/// @param init_state Estado inicial.
			Episode(State *init_state);

            /// Cria um Episode a partir de outro Episode.
            ///
            /// Este construtor cria uma cópia completa de um outro Episode.
			Episode(const Episode &episode);

			/// Destrutor.
			///
			/// Destrói toda a sequência de estados.
			/// @note Este destrutor não destrói os objetos State de um episódio.
			virtual ~Episode();
			
			/// Adiciona um estado na sequência.
			/// @param s Estado para adicionar na sequência.
			void add_state(State *s);
			
			/// Adiciona uma ação na sequência.
			/// @param a Ação para adicionar na sequência.
			void add_action(Action *a);
			
			/// Adiciona uma recompensa na sequência.
			/// @param r Recompensa para adicionar na sequência.
			void add_reward(double r);

			/// Adiciona em uma única chamada a tupla \f$ (a, r, s') \f$ na sequência.
			/// 
			/// É assumido que o estado atual **s** já foi adicionado na sequência.
			/// @param curr_action Ação executada.
			/// @param next_state Próximo estado.
			/// @param r Recompensa recebida.
			void transition(Action *curr_action, State *next_state, double r);

			/// Adiciona em uma única chamada a tupla \f$ (a, r, s') \f$ na sequência.
			/// 
			/// É assumido que o estado atual **s** já foi adicionado na sequência.
			/// @param eo Local de onde a tupla \f$ (a, r, s') \f$ é extraída.
			void transition(EnvOutcome eo);
			
			/// Número de passos realizados neste episódio.
			/// @return Número de passos realizados neste episódio.
			int num_steps();

			/// Calcula o retorno.
			/// @param gamma Desconto das recompensas recebidas.
			/// @return soma das recompensas descontadas por **gamma**.
			double discounted_return(double gamma);
		};
	}
}

#endif
