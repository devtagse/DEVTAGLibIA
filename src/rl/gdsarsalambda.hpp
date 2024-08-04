/*
 gdsarsalambda.hpp
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

#ifndef GDSARSALAMBDA_H
#define GDSARSALAMBDA_H

#include "linearfa.hpp"
#include "episode.hpp"
#include "env.hpp"

#include <random>

namespace ia {
	namespace rl {
		/// Algoritmo Sarsa Lambda com Gradiente Descendente.
		///
		/// Utiliza aproximador de funções baseado no gradiente descendente para
		/// encontrar as melhores estimativas de retorno para o par \f$ (s,a) \f$.
		/// @see ia::rl::Env ia::rl::Episode ia::rl::LinearFA ia::rl::TileCoding
		///
        /// ### Exemplo
        /// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~.cpp
        /// ia::rl::Env *env = new ... // Cria um ambiente Env
		///
		/// ia::rl::TileCoding tc(); // Cria parâmetros livres do aproximador de funções
		/// std::vector<double> widths { 10, 10 }; // Dimensão dos tiles
		/// tc.add_tiling(std::vector<bool> { true, true }, widths, 5); // Adiciona 5 tilings com 2 dimensões
		/// tc.add_tiling(std::vector<bool> { false, true }, widths, 5); // Adiciona 5 tilings com 1 dimensão
		///
		/// ia::rl::LinearFA fa(tc); // Constrói um aproximador de funções linear
    	/// ia::rl::GDSarsaLambda gdsl(0.1, 0.9, 0.95, 0.1, 0.01, true, &fa); // Cria agente baseado no algoritmo Sarsa Lambda
    	/// ia::rl::Episode e = gdsl.run_learning(env, 100); // Inicia aprendizagem de 1 episódio
        /// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		class GDSarsaLambda {
		private:
			std::default_random_engine m_gen; ///< Gerador de números aleatórios.
			std::uniform_real_distribution<float> m_distribution; ///< Distribuição uniforme entre 0 e 1.

		public:
			LinearFA *m_vfa; ///< Aproximador de funções linear.

			double m_alpha; ///< Taxa de aprendizagem.
			double m_lambda; ///< Taxa de decaimento.
			double m_gamma; ///< Desconto do retorno.
			double m_E; ///< Exploração.
			double m_min_lambda; ///< Valor mínimo do traço de elegibilidade.
			bool m_replace_traces; ///< Utilizar substituição do traço de elegibilidade.

			int m_curr_step; ///< Passo de tempo corrente

			/// Cria um agente Sarsa Lambda de aprendizagem por reforço.
			/// @param alpha Taxa de aprendizagem.
			/// @param lambda Taxa de decaimento.
			/// @param gamma Desconto do retorno.
			/// @param E Exploração.
			/// @param min_lambda Valor mínimo do traço de elegibilidade.
			/// @param replace_traces Utilizar substituição do traço de elegibilidade.
			/// @param vfa Aproximador de funções linear.
			/// @see ia::rl::LinearFA
			/// @note A liberação da memória utilizada pelo aproximador linear **vfa** não é liberada com
			/// a destruição de um objeto GDSarsaLambda. 
			GDSarsaLambda(double alpha, double lambda, double gamma, double E, double min_lambda, bool replace_traces, LinearFA *vfa);
			
			virtual ~GDSarsaLambda() { }
			
			/// Inicia a aprendizagem do agente em um episódio.
			/// @param env Ambiente de aprendizagem por reforço.
			/// @param max_steps Número máximo de passos permitidos no episódio.
			/// @return Um objeto Episode com dados relativos à aprendizagem do agente durante o episódio.
			/// @see ia::rl::Episode ia::rl::Env
			/// @note O ambiente não é reiniciado após o fim do episódio por esta função. Chame manualmente a 
			/// função ia::rl::Env::reset_env() antes de chamar run_learning() novamente, caso necessário.
			Episode *run_learning(Env *env, int max_steps);

			/// Retorna uma ação seguindo uma política E-greedy.
			/// @param env Ambiente de aprendizagem por reforço.
			/// @param curr_s Estado do ambiente.
			/// @return Melhor ação **a** de acordo com a estimativa atual de \f$ Q(s,a) \f$ ou
			/// uma ação aleatório com probabilidade **E**.
			Action *egreedy_action(Env *env, State *curr_s);

			/// Encontra a melhor ação de acordo com a estimativa atual do valor Q.
			/// @param env Ambiente de aprendizagem por reforço.
			/// @param curr_s Estado do ambiente.
			/// @return Melhor ação **a** de acordo com a estimativa atual de \f$ Q(s,a) \f$;
			Action *greedy_action(Env *env, State *curr_s);
		};
	}
}

#endif
