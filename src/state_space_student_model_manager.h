#ifndef ANALYSIS_COMMON_R_BSTS_SRC_STATE_SPACE_STUDENT_MODEL_MANAGER_H_
#define ANALYSIS_COMMON_R_BSTS_SRC_STATE_SPACE_STUDENT_MODEL_MANAGER_H_

#include "model_manager.h"
#include "LinAlg/Matrix.hpp"
#include "LinAlg/Vector.hpp"
#include "Models/StateSpace/StateSpaceStudentRegressionModel.hpp"

namespace BOOM {
namespace bsts {

class StateSpaceStudentHoldoutErrorSampler
    : public HoldoutErrorSamplerImpl {
 public:
  // Args:
  //   model:  The model containing data up to a specified cutpoint.
  //   holdout_responses:  Observed values after the cutpoint.
  //   holdout_predictors: A matrix of observed predictors corresponding to
  //     holdout_responses.
  //   niter: The desired number of draws (MCMC iterations) from the posterior
  //     distribution.
  //   errors:  A matrix that will hold the output of the simulation.
  StateSpaceStudentHoldoutErrorSampler(
      const Ptr<StateSpaceStudentRegressionModel> &model,
      const Vector &holdout_responses,
      const Matrix &holdout_predictors,
      int niter,
      Matrix *errors)
      : model_(model),
        holdout_responses_(holdout_responses),
        holdout_predictors_(holdout_predictors),
        niter_(niter),
        errors_(errors) {
    rng_.seed(seed_rng());
  }

  void sample_holdout_prediction_errors() override {
    model_->sample_posterior();
    errors_->resize(niter_,
                    model_->time_dimension() + holdout_responses_.size());
    for (int i = 0; i < niter_; ++i) {
      model_->sample_posterior();
      Vector all_errors = model_->one_step_prediction_errors();
      all_errors.concat(model_->one_step_holdout_prediction_errors(
          rng_, holdout_responses_, holdout_predictors_,
          model_->final_state()));
      errors_->row(i) = all_errors;
    }
  }

 private:
  Ptr<StateSpaceStudentRegressionModel> model_;
  Vector holdout_responses_;
  Matrix holdout_predictors_;
  int niter_;
  Matrix *errors_;
  RNG rng_;
};

// The model manager for a student regression model.
class StateSpaceStudentModelManager
    : public ModelManager {
 public:
  StateSpaceStudentModelManager();

  StateSpaceStudentRegressionModel * CreateObservationModel(
      SEXP r_data_list,
      SEXP r_prior,
      SEXP r_options,
      RListIoManager *io_manager) override;

  HoldoutErrorSampler CreateHoldoutSampler(
      SEXP r_bsts_object,
      int cutpoint,
      Matrix *prediction_error_output) override;

  void AddDataFromBstsObject(SEXP r_bsts_object) override;
  void AddDataFromList(SEXP r_data_list) override;
  int UnpackForecastData(SEXP r_prediction_data) override;
  Vector SimulateForecast(const Vector &final_state) override;

  void SetPredictorDimension(int xdim);

 private:
  void AddData(const Vector &response,
               const Matrix &predictors,
               const std::vector<bool> &response_is_observed);

  Ptr<StateSpaceStudentRegressionModel> model_;
  int predictor_dimension_;

  Matrix forecast_predictors_;
};

}  // namespace bsts
}  // namespace BOOM

#endif  // ANALYSIS_COMMON_R_BSTS_SRC_STATE_SPACE_STUDENT_MODEL_MANAGER_H_
