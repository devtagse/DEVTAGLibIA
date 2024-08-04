/*
 linearfa.hpp
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

#ifndef LINEARFA_H
#define LINEARFA_H

#include "tilecoding.hpp"
#include "state.hpp"
#include "action.hpp"

#include <vector>
#include <map>
#include <string>
#include <fstream>
#include <iostream>

namespace ia {
	namespace rl {
		/// Classe realiza o mapeamento do id de um parâmentro livre em outro id.
		/// @see get_or_create()
		class FeaturesMap {
		private:
			std::map<int, int> m_features_map; ///< Mapeamento do id de parâmetro livre em outro id.

		public:
			/// Cria um FeaturesMap sem nenhum mapeamento.
			FeaturesMap();

            /// Cria um FeaturesMap a partir de outro FeaturesMap.
            ///
            /// Este construtor cria uma cópia completa de um outro FeaturesMap.
			FeaturesMap(const FeaturesMap &features_map);

			virtual ~FeaturesMap() { }

			/// Cria ou recupera um parâmetro livre.
			///
			/// Esta função faz o mapeamento do id de um parâmentro livre em outro id.
			/// @param from Id do parâmetro livre que se deseja fazer o mapeamento.
			/// @param feature_id Contador que enumera os parâmetros livres.
			/// @return Novo id resultado do mapeamento do parâmetro livre **from** em outro id.
			int get_or_create(int from, int &feature_id);

			/// Carrega os parâmetros livres de um LinearFA.
			/// @note Não utilize este operador em FeaturesMap, ao invés disto,
			/// use em LinearFA.
			/// @see operator>>(std::ostream, LinearFA)
			friend std::istream& operator>>(std::istream& is, FeaturesMap& fm) {
				// FeaturesMap
				char c = is.get(); // (
				// ((iii,iii),(iii,iii))
				do {
					int key;
					int value;
					std::string line;

					is.get(); // (
					getline(is, line, ','); // ,
					key = stoi(line);
					getline(is, line, ')'); // )
					value = stoi(line);
					c = is.get(); // , or )

					fm.m_features_map[key] = value;
				} while(c != ')');
				return is;
			}

			/// Salva os parâmetros livres de um LinearFA.
			/// @note Não utilize este operador em FeaturesMap, ao invés disto,
			/// use em LinearFA.
			/// @see operator<<(std::ostream, LinearFA)
			friend std::ostream& operator<<(std::ostream& os, const FeaturesMap& fm) {
				auto it = fm.m_features_map.begin();
				auto it2 = --fm.m_features_map.end();
				os << "(";
				for(; it != it2; ++it)
					os << "(" << it->first << "," << it->second << "),";
				return os << "(" << it->first << "," << it->second << "))";
			}
		};

		/// Classe responsável pelo produto \f$ s \times a \f$.
		///
		/// Todo id de parâmetro livre gerado por CrossProductFeature está associado a um par \f$ (s,a) \f$.
		/// @see ia::rl::LinearFA ia::rl::GDSarsaLambda
		class CrossProductFeatures {
		private:
			TileCoding m_sfeatures; ///< Modelo de organização dos parâmetros livres TileCoding.
			/// Mapeamento dos parâmetros livres de um estado **s** para o par \f$ (s,a) \f$.
			std::map<int, FeaturesMap> m_action_features;
			/// Mantém um id único para cada parâmetro livre do modelo.
			/// @see FeatureMap::get_or_create()
			int m_feature_id;

			/// Realiza o mapeamento dos ids dos parâmetros livres de um estado **s** para o par \f$ (s,a) \f$.
			/// @param a Ação **a** do par \f$ (s,a) \f$ que se deseja obter os parâmetros livres.
			/// @param from Id do parâmetro livre do estado **s** que se deseja converter para o par \f$ (s,a) \f$.
			/// @return Id de um parâmetro livre do estado **s** convertido para o par \f$ (s,a) \f$.
			int action_feature(Action &a, int from);

		public:
			/// Cria um CrossProductFeatures com um modelo de organização TileCoding para os parâmetros livres.
			/// @param sfeatures Modelo TileCoding.
			/// @see TileCoding
			CrossProductFeatures(TileCoding sfeatures);

            /// Cria um CrossProductFeatures a partir de outro CrossProductFeatures.
            ///
            /// Este construtor cria uma cópia completa de um outro CrossProductFeatures.
			CrossProductFeatures(const CrossProductFeatures &cross_pfeatures);

			virtual ~CrossProductFeatures() { }

			/// Obtém todos os parâmetros livres relacionados ao par \f$ (s,a) \f$
			/// @param s Estado **s** do par \f$ (s,a) \f$ que se deseja obter os parâmetros livres.
			/// @param a Ação **a** do par \f$ (s,a) \f$ que se deseja obter os parâmetros livres.
			/// @return Vetor com conjunto de parametros livres associados ao par \f$ (s,a) \f$
			std::vector<StateFeature> features(State *s, Action &a);

			/// Calcula a quantidade de parametros livres utilizados até o momento.
			/// @return Quantidade de parâmetros livres utilizados.
			int num_features();

			/// Carrega os parâmetros livres de um LinearFA.
			/// @note Não utilize este operador em CrossProductFeatures, ao invés disto,
			/// use em LinearFA.
			/// @see operator>>(std::ostream, LinearFA)
			friend std::istream& operator>>(std::istream& is, CrossProductFeatures& cpf) {
				// CrossProductFeatures
				is.get(); // (
				std::string line;
				getline(is, line, ','); // ,
				cpf.m_feature_id = stoi(line);
				is >> cpf.m_sfeatures; // ...
				is.get(); // ,

				// (...,((iii,...),(iii,...)))
				char c = is.get(); // (
				do {
					int key;
					FeaturesMap fm;
					std::string line;

					is.get(); // (
					getline(is, line, ','); // (
					key = stoi(line);
					is >> fm; // ...
					is.get(); // )
					c = is.get(); // , or )

					cpf.m_action_features[key] = fm;
				} while(c != ')');
				is.get(); // )
				return is;
			}

			/// Salva os parâmetros livres de um LinearFA.
			/// @note Não utilize este operador em CrossProductFeatures, ao invés disto,
			/// use em LinearFA.
			/// @see operator<<(std::ostream, LinearFA)
			friend std::ostream& operator<<(std::ostream& os, const CrossProductFeatures& cpf) {
				os << "(" << cpf.m_feature_id  << "," << cpf.m_sfeatures;
				auto it = cpf.m_action_features.begin();
				auto it2 = --cpf.m_action_features.end();
				os << ",(";
				for(; it != it2; ++it) 
					os << "(" << it->first << "," << it->second << "),";
				return os << "(" << it->first << "," << it->second << ")))";
			}
		};

		/// Associa cada parâmetro livre do modelo TileCoding ao seu respectivo peso.
		///
		/// Utilize-a também para calcular os valores **Q** do par \f$ (s,a) \f$.
		/// @see ia::rl::TileCoding
		class LinearFA {
		private:
			std::vector<StateFeature> m_curr_features; ///< Últimos parâmetros livres consultados pela função evaluate().
			double m_curr_value; ///< Último valor calculado pela função evaluate().
			std::map<int, double> m_curr_gradient; ///< Últimos pesos consultados pela função evaluate().

			State *m_last_state; ///< Último estado recebido.
			Action *m_last_action; ///< Última ação recebida.

			CrossProductFeatures m_safeatures; ///< Parâmetros livres de um par \f$ (s,a) \f$.
			double m_default_weight; ///< Peso inicial de todos os parâmetros livres.
		public:
			std::map<int, double> m_weights; ///< Mapeamento de um parâmetro livre ao seu respectivo peso.

			/// Cria um aproximador de funções linear sem nenhuma configuração do gerador de parâmetros livres TileCoding.
			/// @note Este construtor só deve ser chamado para criar um aproximador de funções para
			/// realizar lei
			LinearFA();

			/// Cria um aproximador de funções linear com os pesos dos parâmetros livres iniciando com valor 0.
			/// @param tilecoding Gerador de parâmetros livres para um par \f$ (s,a) \f$.
			LinearFA(TileCoding tilecoding);

			/// Cria um aproximador de funções linear com os pesos dos parâmetros livres iniciando com valor definido
			/// pelo parâmetro **default_weight**.
			/// @param tilecoding Gerador de parâmetros livres para um par \f$ (s,a) \f$.
			/// @param default_weight Peso inicial para todos os parâmetros livres.
			LinearFA(TileCoding tilecoding, double default_weight);

			virtual ~LinearFA() { }

			/// Calcula o valor **Q** do par \f$(s,a) \f$.
			/// @param s Estado **s** do par \f$(s,a) \f$ que se deseja obter o valor **Q**.
			/// @param a Ação **a** do par \f$(s,a) \f$ que se deseja obter o valor **Q**.
			/// @return Valor \f$ Q(s,a) \f$.
			double evaluate(State &s, Action &a);

			/// Obtém todos os parâmetros livres e seus respectivos pesos associados ao par \f$ (s,a) \f$
			/// @param s Estado **s** do par \f$ (s,a) \f$ que se deseja obter os parâmetros livres e seus respectivos pesos.
			/// @param a Ação **a** do par \f$ (s,a) \f$ que se deseja obter os parâmetros livres e seus respectivos pesos.
			/// @return Mapeamento do id do parâmetro livre com seu respectivo peso.
			std::map<int, double> gradient(State *s, Action *a);

			/// Obtém a quantidade de parâmetros livres utilizados.
			/// @return Quantidade de parâmetros livres utilizados.
			int num_param();

			/// Obtém o peso de um parâmetro livre.
			/// @param weight_id Id do parâmetro livre.
			/// @return Peso do parâmetro livre.
			double get_weight(int weight_id);

			/// Limpa o peso de todos os parâmetros livres.
			void reset_params();

			/// Carrega os parâmetros livres de um LinearFA.
			///
			/// Este operador pode ser utilizado para ler de um arquivo um LinearFA
			/// salvo anteriormento pelo operador <<.
			/// @see operator<<(std::istream, LinearFA)
			///
			/// ### Exemplo
			/// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~.cpp
			/// std::ifstream ifs;
			/// ifs.open("motor_ctrl.train");
			/// ia::rl::LinearFA lfa();
			/// ifs >> lfa;
			/// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			friend std::istream& operator>>(std::istream& is, LinearFA& lfa) {
				// CrossProductFeatures
				is.get(); // (
				is >> lfa.m_safeatures; // ...
				is.get(); // ,

				// (...,((iii,ddd),(iii,ddd)))
				char c = is.get(); // (
				do {
					int key;
					double value;
					std::string line;

					is.get(); // (
					getline(is, line, ','); // ,
					key = stod(line);
					getline(is, line, ')'); // )
					value = stod(line);
					c = is.get(); // , or )

					lfa.m_weights[key] = value;
				} while(c != ')');
				is.get(); // )
				return is;
			}

			/// Salva os parâmetros livres de um LinearFA.
			///
			/// Este operador pode ser utilizado para salvar um treinamento armazenado
			/// nos parâmetros livres de um LinearFA.
			/// @see operator>>(std::ostream, LinearFA)
			///
			/// ### Exemplo
			/// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~.cpp
			/// ia::rl::GDSarsaLambda *agent = new ... // Cria um novo agente
			/// ... // Código para treinar o agente
			/// ofstream out;
			/// out.open("motor_ctrl.train"); // Abre um arquivo para escrita
			/// out << *gdsl->m_vfa; // Escreve o aproximador linear no arquivo
			/// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			friend std::ostream& operator<<(std::ostream& os, const LinearFA& lfa) {
				os << "(" << lfa.m_safeatures;
				auto it = lfa.m_weights.begin();
				auto it2 = --lfa.m_weights.end();
				os << ",(";
				for(; it != it2; ++it)
					os << "(" << it->first << "," << it->second << "),";
				return os << "(" << it->first << "," << it->second << ")))";
			}
		};
	}
}

#endif