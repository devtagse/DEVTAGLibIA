/*
 astar.cpp
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

#include <algorithm>
#include <queue>
#include <unordered_set>
#include <vector>
#include <map>
#include <memory>

#include "astar.hpp"

using namespace ia::astar;

namespace std {
    template <>
    struct hash<slam::Point2D<int>> {
        size_t operator()(const slam::Point2D<int>& point) const {
            return std::hash<int>()(point.m_x) ^ std::hash<int>()(point.m_y);
        }
    };
}

AStar::AStar(AStarEnv *env) : m_env(env) {}

AStar::AStar(const AStar &astar) : m_env(astar.m_env) {}

AStar::~AStar() {}

std::vector<slam::Point2D<int>> AStar::path_to(slam::Point2D<int> from, slam::Point2D<int> to) {
    // from deve estar na mesma granularidade de m_env
    from = from/m_env->get_plan_div();
    m_env->set_goal(to); // set_goal coloca to na mesma granularidade de m_env

    std::vector<slam::Point2D<int>> path_points;
    if (m_env->is_obstacle(m_env->get_goal()) || from.distance(to) < m_env->get_plan_div())
        return path_points;

    auto comp = [](const std::shared_ptr<ia::astar::AStarNode>& a, const std::shared_ptr<ia::astar::AStarNode>& b) {
        return a->fcost() > b->fcost();
    };
    std::priority_queue<std::shared_ptr<ia::astar::AStarNode>, std::vector<std::shared_ptr<ia::astar::AStarNode>>, decltype(comp)> open_set(comp);
    std::unordered_set<slam::Point2D<int>> closed_set;

    open_set.emplace(std::make_shared<ia::astar::AStarNode>(from, m_env->cost_to_goal(from)));
    std::shared_ptr<ia::astar::AStarNode> curr_node = nullptr;
    while (!open_set.empty()) {
        curr_node = open_set.top();
        open_set.pop();

        if (m_env->is_goal(curr_node->m_pos))
            break;

        if (closed_set.find(curr_node->m_pos) != closed_set.end())
            continue;

        closed_set.insert(curr_node->m_pos);

        std::vector<int> actions = m_env->applicable_actions(curr_node->m_pos);
        for (int action : actions) {
            slam::Point2D<int> next_pos = m_env->transition(curr_node->m_pos, action);
            if (closed_set.find(next_pos) == closed_set.end()) {
                auto new_node = std::make_shared<ia::astar::AStarNode>(next_pos, m_env->action_cost(action) + m_env->cost_to_goal(next_pos)+m_env->count_near_obstacles(next_pos, 2), curr_node);
                open_set.emplace(new_node);
            }
        }
    }

    if (!m_env->is_goal(curr_node->m_pos)) // Caminho não encontrado
        return path_points;

    while (curr_node != nullptr) {
        path_points.push_back(curr_node->m_pos*m_env->get_plan_div());
        curr_node = curr_node->m_parent;
    }

    path_points.pop_back(); // Remove o ponto inicial
    return path_points;
}