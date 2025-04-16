#include <algorithm>
#include <vector>
#include "envoutcome.hpp"

namespace ia {
	namespace rl {
        class ExperienceReplay {
            std::vector<EnvOutcome> m_memory;
            int m_next = 0;
            int m_size = 0;

            bool m_alwaysIncludeMostRecent;

            ExperienceReplay(int size) {
                m_memory.resize(size);
            }

            ExperienceReplay(int size, bool alwaysIncludeMostRecent) {
                if(size < 1)
                    throw std::runtime_error("FixedSizeMemory requires memory size > 0; was request size of " + size);
                m_alwaysIncludeMostRecent = alwaysIncludeMostRecent;
                m_memory.resize(size);
            }

            bool alwaysIncludeMostRecent() {
                return alwaysIncludeMostRecent;
            }

            void setAlwaysIncludeMostRecent(bool alwaysIncludeMostRecent) {
                m_alwaysIncludeMostRecent = alwaysIncludeMostRecent;
            }

            void addExperience(EnvOutcome eo) {
                m_memory[m_next] = eo;
                m_next = (m_next+1) % m_memory.size();
                m_size = std::min(m_size+1, m_memory.size());
            }

            std::vector<EnvOutcome> sampleExperiences(int n) {
                std::vector<EnvOutcome> samples;

                if(m_size == 0)
                    return std::vector<EnvOutcome>();

                if(m_alwaysIncludeMostRecent)
                    n--;

                if(m_size < n){
                    samples.resize(m_size);
                    for(int i = 0; i < m_size; i++)
                        samples[i] = m_memory[i];
                    return samples;
                }
                else {
                    samples.resize(std::max(n, 1));
                    for(int i = 0; i < n; i++) {
                        int sind = (int)(rand()/(double)RAND_MAX * m_size);
                        samples.push_back(m_memory[sind]);
                    }
                }
                if(m_alwaysIncludeMostRecent){
                    if(m_next > 0)
                        samples.push_back(m_memory[m_next - 1]);
                    else if(m_size > 0)
                        samples.push_back(m_memory[m_memory.size()-1]);
                    else
                        throw std::runtime_error("FixedSizeMemory getting most recent fails because memory is size 0.");
                }

                return samples;
            }

            void resetMemory() {
                m_size = 0;
                m_next = 0;
            }
        };
    }
}