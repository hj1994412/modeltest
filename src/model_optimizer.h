/*
 * model_optimizer.h
 *
 *  Created on: Jun 10, 2015
 *      Author: diego
 */

#ifndef MODEL_OPTIMIZER_H_
#define MODEL_OPTIMIZER_H_

#include "model.h"
#include "thread/observer.h"

namespace modeltest
{

  typedef enum {
      mt_param_branch_lengths,
      mt_param_subst_rates,
      mt_param_frequencies,
      mt_param_alpha,
      mt_param_pinv,
  } mt_parameter_t;

  extern bool on_run;

  class ModelOptimizer : public Observable
  {
  public:
    ModelOptimizer (Model *_model,
                    const partition_descriptor_t & _partition,
                    mt_index_t _thread_number = 0)
        : model(_model),
          partition(_partition),
          thread_number(_thread_number)
    {
        interrupt_optimization = false;
        optimized = false;
    }
    virtual ~ModelOptimizer ();

    /**
     * @brief Optimizes one single parameter
     * @param[in] which_parameter   parameter to optimize
     * @param[in] tolerance         tolerance for parameter optimization
     * @return the resulting log-Likelihood of the model
     */
    virtual double opt_single_parameter(mt_parameter_t which_parameter,
                                        double tolerance = DEFAULT_PARAM_EPSILON,
                                        bool first_guess = false) = 0;

    /**
     * @brief Optimizes branch lengths
     * @param[in] tolerance         tolerance for parameter optimization
     * @return the resulting log-Likelihood of the model
     */
    virtual double opt_branch_lengths(double tolerance = DEFAULT_PARAM_EPSILON) = 0;

    /**
     * @brief Optimizes Gamma shape
     * @param[in] tolerance         tolerance for parameter optimization
     * @return the resulting log-Likelihood of the model
     */
    virtual double opt_alpha(double tolerance = DEFAULT_PARAM_EPSILON,
                             bool first_guess = false) = 0;

    /**
     * @brief Optimizes proportion of invariant sites
     * @param[in] tolerance         tolerance for parameter optimization
     * @return the resulting log-Likelihood of the model
     */
    virtual double opt_pinv(double tolerance = DEFAULT_PARAM_EPSILON,
                             bool first_guess = false) = 0;

    /**
     * @brief Optimizes all parameters for the model
     * @param[in] epsilon     the tolerance of the global optimization
     * @param[in] tolerance   tolerance for parameter optimization
     * @param[in] num_threads number of threads for model optimization
     * @return true, if the optimization is OK
     */
    virtual bool run(double epsilon   = DEFAULT_OPT_EPSILON,
                     double tolerance = DEFAULT_PARAM_EPSILON,
                     mt_size_t num_threads = 1) = 0;

    /**
     * @brief Gets the optimization status of the model
     * @return true, if the model is optimized
     */
    int is_optimized() const { return optimized; }

    mt_index_t get_thread_number() const { return thread_number; }
    mt_index_t get_cur_parameter() const { return cur_parameter; }
    double get_opt_delta() const { return opt_delta; }
    void interrupt() { interrupt_optimization = true; }

  protected:
    bool optimized; //! optimization state
    Model *model;   //! the model to optimize
    const partition_descriptor_t partition;

    mt_index_t thread_number;  //! the number of the current thread

    // dynamic optimization status
    bool interrupt_optimization;
    mt_index_t cur_parameter;
    double opt_delta;
  };

} /* namespace modeltest */

#endif /* MODEL_OPTIMIZER_H_ */
