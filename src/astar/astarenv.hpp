/*
 astarenv.hpp
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

#ifndef ASTARENV_H
#define ASTARENV_H

#include <point2d.hpp>

#include <vector>

namespace ia {
	namespace astar {
        /// Estrutura de um ambiente para busca heurística.
        /// @see ia::astar::AStar
        class AStarEnv {
        private:
            int m_num_actions; ///< Números de ações permitidas no ambiente.

        public:
            /// Cria um AStarEnv com um objetivo inicial e um número de ações permitidas no ambiente.
            ///
            /// O ponto que define o objetivo pode ser alterado posteriormente utilizando a função set_goal().
            /// Sendo assim, não é necessário criar um novo objeto para cada objetivo.
            /// @param num_actions Número de ações permitidas no ambiente.
            AStarEnv(int num_actions) : m_num_actions(num_actions) {}

            /// Cria um AStarEnv a partir de outro AStarEnv.
            ///
            /// Este construtor cria uma cópia completa do ambiente.
            AStarEnv(const AStarEnv &astar_env) : m_num_actions(astar_env.m_num_actions) {}

            virtual ~AStarEnv() { }

            /// Obtém o número de ações permitidas no ambiente.
            /// @return Quantidade de ações.
            virtual int num_actions()
            {
                return m_num_actions;
            }

            /// Verifica quais ações podem ser executadas em um determinado ponto.
            /// @param pos Ponto para verificar as ações disponíveis.
            /// @return Um std::vector<int> com o número das ações disponíveis.
            virtual std::vector<int> applicable_actions(slam::Point2D<int> pos)
            {
                std::vector<int> acts;
                int n_acts = num_actions();

                for (int i = 0; i < n_acts; i++) {
                    slam::Point2D<int> next = transition(pos, i);
                    if (!is_obstacle(next))
                        acts.push_back(i);
                }
                return acts;
            }
            
            /// Calcula o custo a partir de um ponto para alcançar o objetivo.
            ///
            /// Esta função calcula a distância euclidiana entre **pos** e o objetivo.
            /// @param pos Ponto para verificar o custo.
            /// @return Distância euclidiana entre **pos** e o objetivo
            /// @see AStarNode::fcost()
            virtual double cost_to_goal(slam::Point2D<int> pos) = 0;

            /// Função retorna o custo em se realizar uma determinada ação
            ///
            /// Esta função calcula o custo em se realizar a ação **action**.
            /// @param action Ação para se determinar o custo.
            /// @return Custo em se realizar uma ação.
            virtual double action_cost(int action) = 0;

            /// Define um novo objetivo.
            /// @param goal Ponto que representa o novo objetivo.
            virtual void set_goal(slam::Point2D<int> goal) = 0;

            // Retorna objetivo atual
            /// @return Objetivo.
            virtual slam::Point2D<int> get_goal() = 0;

            /// Verifica se um ponto é o objetivo.
            /// @param pos Ponto para verificar se é o objetivo.
            /// @return **true** se o ponto **pos** é o objetivo ou **false**, caso contrário.
            virtual bool is_goal(slam::Point2D<int> pos) = 0;

            /// Calcula o próximo ponto a partir de um anterior após a execução de uma ação.
            /// @param pos Ponto anterior.
            /// @param action Ação realizada no ponto **pos**.
            /// @return Novo ponto após execução de uma ação.
            virtual slam::Point2D<int> transition(slam::Point2D<int> pos, int action) = 0;

            /// Verifica se o ponto **pos** é um obstáculo.
            /// @param pos Ponto para verificar se é um obstáculo.
            /// @return **true** se o ponto **pos** é um obstátulo ou **false**, caso contrário.
            virtual bool is_obstacle(slam::Point2D<int> pos) = 0;

            /// Verifica se o ponto **pos** é um obstáculo. Arredonda utilizando round().
            /// @param pos Ponto para verificar se é um obstáculo.
            /// @return **true** se o ponto **pos** é um obstátulo ou **false**, caso contrário.
            virtual bool is_obstacle(slam::Point2D<double> pos) = 0;

            /// Conta a quantidade de obstáculos próximos a um ponto.
            /// @param pos Posição para verificar os obstáculos próximos.
            /// @param radius Raio de busca.
            /// @return Quantidade de obstáculos próximos.
            virtual int count_near_obstacles(slam::Point2D<int> pos, int radius) = 0;

            /// Retorna a divisão do mapa.
            /// @return Divisão do mapa.
            virtual int get_plan_div() = 0;
        };
    }
}

#endif