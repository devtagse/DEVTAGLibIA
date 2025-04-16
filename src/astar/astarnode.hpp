/*
 astarnode.hpp
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

#ifndef ASTARNODE_H
#define ASTARNODE_H

#include <memory>

#include "point2d.hpp"

namespace ia {
	namespace astar {
        /// Classe auxiliar utilizada para encontrar o melhor caminho pelo Algotimo A*.
        ///
        /// Cada ponto no espaço do ambiente é um AStarNode. Esta classe calcula
        /// o custo do ponto **m_pos** até o objetivo. O custo de transição
        /// para um outro nó é unitário, *i.e.*, vale 1.
        /// @see ia:astar::AStar
        class AStarNode {
        protected:
            /// Custo H calculado pelo ambiente.
            /// @see ia::astar::AStarEnv::cost_to_goal()
            double m_hcost;

        public:
            slam::Point2D<int> m_pos; ///< Local deste AStarNode no ambiente.
            std::shared_ptr<AStarNode> m_parent; ///< Ponteiro para o AStarNode anterior.

            /// Cria um AStarNode.
            /// @param pos Local deste AStarNode no ambiente.
            /// @param fcost Custo do ponto **pos** até o objetivo.
            /// @param parent 
            /// @see ia::astar::AStarEnv::cost_to_goal()
            AStarNode(slam::Point2D<int> pos, double hcost, std::shared_ptr<AStarNode> parent) : m_pos(pos), m_hcost(hcost), m_parent(parent) {}

            /// Cria um AStarNode como primeiro nó.
            ///
            /// Este construtor será a origem e, portanto, **m_parent** é NULL.
            /// @param pos Local deste AStarNode no ambiente.
            /// @param fcost Custo do ponto **m_pos** até o objetivo.
            AStarNode(slam::Point2D<int> pos, double hcost) : AStarNode(pos, hcost, nullptr) {}

            /// Cria um AStarNode a partir de outro AStarNode.
            ///
            /// Este construtor cria uma cópia completa de um outro AStarNode
            /// incluindo a referência de **m_parent** para o nó anterior.
            AStarNode(const AStarNode &node) : m_pos(node.m_pos), m_hcost(node.m_hcost), m_parent(node.m_parent) {}
            
            /// Cria um AStarNode no ponto (0,0).
            AStarNode() : m_pos(0, 0), m_hcost(0), m_parent(nullptr) {}

            /// Destrutor.
            /// @note Este destrutor não destrói o objeto AStarNode em **m_parent**.
            virtual ~AStarNode() {}

            /// Calcula recursivamente o custo G.
            /// @return Custo G.
            virtual double gcost()
            {
                if (m_parent != nullptr)
                    return m_parent->gcost() + 1;
                else
                    return 1;
            }

            /// Calcula o custo F.
            /// @see ia::astar::AStarEnv::cost_to_goal()
            virtual double fcost()
            {
                return gcost() + m_hcost;
            }

            /// Compara dois AStarNode pelo custo para alcançar o objetivo.
            /// @return **true** caso o custo do AStarNode à esquerda seja menor que o da
            /// direita ou **false**, caso contrário.
            /// @see fcost()
            bool operator<(AStarNode& rhs)
            {
                return fcost() < rhs.fcost();
            }
        };
    }
}

#endif