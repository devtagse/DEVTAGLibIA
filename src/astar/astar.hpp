/*
 astar.hpp
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

#ifndef ASTAR_H
#define ASTAR_H

#include <point2d.hpp>
#include "astarnode.hpp"
#include "astarenv.hpp"

#include <map>
#include <vector>

/// Inteligência artificial.
namespace ia {
    /// Algoritmo A*.
	namespace astar {
        /// Implementação do Algoritmo A*.
        ///
        /// Encontra o melhor caminho entre dois pontos de um ambiente AStarEnv utilizando o
        /// algoritmo A*.
        /// @see ia::astar::AStarEnv
        ///
        /// ### Exemplo
        /// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~.cpp
        /// ia::astar::AStar path_plan(env); // env é um ambiente AStarEnv
        ///
        /// slam::Point2D<int> to(0,0); // Ponto de partida
        /// slam::Point2D<int> from(10,10); // Ponto de chegada
        ///
        /// std::vector<slam::Point2D<int>> path = path_plan.path_to(to, from); // Obtém o subconjunto de pontos que interligam os pontos to à from
        /// while(!path.empty()) {
        ///     std::cout << path.back() << std::endl; // Mostra o caminho na ordem **to** até **from**
        ///     path.pop_back();
        /// }
        /// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        class AStar {
        public:
            AStarEnv *m_env; ///< Ambiente de busca.
            
            /// Cria um AStar.
            ///
            /// @param env Ambiente do tipo AStarEnv.
            AStar(AStarEnv *env);

            /// Cria um AStar a partir de outro AStar.
            ///
            /// Este construtor não cria uma cópia do ambiente em m_env, apenas a
            /// referência para o ambiente.
            AStar(const AStar &astar);

            /// Destrutor AStar.
            /// @note Este destrutor não destrói o objeto AStarEnv em **m_env**.
            virtual ~AStar();

            /// Encontra o melhor caminho entre os pontos **to** e **from**.
            ///
            /// Esta função recebe dois pontos e retorna um caminho, possivelmente mais curto, 
            /// que não passa pelos obstáculos do ambiente.
            /// @param from Ponto de partida.
            /// @param to Ponto de chegada.
            /// @return Um **std::vector<slam::Point2D<int>>** com pontos que indicam o caminho entre
            /// **to** e **from**. Os pontos iniciais estão no fim do vetor.
            /// @note Esta função retorna um vetor vazio quando:
            /// * Os pontos **to** e **from** são iguais;
            /// * O ponto **from** é um obstáculo;
            /// * Não há um caminho disponível para alcançar **from** a partir de **to**.
            /// @see slam::Point2D 
            std::vector<slam::Point2D<int>> path_to(slam::Point2D<int> from, slam::Point2D<int> to);
        };
    }
}

#endif