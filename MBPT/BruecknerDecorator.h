#ifndef BRUECKNER_DECORATOR_H
#define BRUECKNER_DECORATOR_H

#include "HartreeFock/HFOperator.h"
#include "SigmaPotential.h"
#include "CoreMBPTCalculator.h"

/** BruecknerDecorator adds one-body "sigma" operators to a HF potential.
    It stores a mapping from kappa to SigmaPotential, and can read and write sigmas to file.
    Creation is deferred to a CoreMBPTCalculator that must be supplied as necessary.

    The sigma potential is non-local, so it is added with the exchange part.
 */
class BruecknerDecorator : public HFOperatorDecorator
{
public:
    BruecknerDecorator(pHFOperator wrapped_hf, pOPIntegrator integration_strategy = nullptr);

    void IncludeLower(bool include_fg = false, bool include_gg = false)
    {   use_fg = include_fg;
        use_gg = include_gg;
    }

    /** Calculate sigma for given kappa if it does not already exist. */
    void CalculateSigma(int kappa, pBruecknerSigmaCalculator brueckner_calculator);

    /** Calculate sigma for given kappa if it does not already exist.
        If bare_hf is not given, then use decorated HFOperator.
     */
    void CalculateSigma(int kappa, pOrbitalManagerConst orbitals, pHartreeY hartreeY, pSpinorOperatorConst bare_hf = nullptr);

    /** Set the scaling parameter. */
    void SetSigmaScaling(double sigma_amount) { lambda = sigma_amount; }
    double GetSigmaScaling() const { return lambda; }

    /** Attempt to read sigma with given kappa, filename is "identifier.[kappa].sigma". */
    void Read(const std::string& identifier, int kappa);

    /** Write sigmas, filename "identifier.[kappa].sigma". */
    void Write(const std::string& identifier, int kappa) const;

    /** Write all sigmas; filenames "identifier.[kappa].sigma". */
    void Write(const std::string& identifier) const;

public:
    virtual void Alert() override;

    /** Set exchange (nonlocal) potential and energy for ODE routines. */
    virtual void SetODEParameters(const SingleParticleWavefunction& approximation) override;

    /** Get exchange (nonlocal) potential. */
    virtual SpinorFunction GetExchange(pSingleParticleWavefunctionConst approximation) const override;

    virtual void GetODEFunction(unsigned int latticepoint, const SpinorFunction& fg, double* w) const override;
    virtual void GetODECoefficients(unsigned int latticepoint, const SpinorFunction& fg, double* w_f, double* w_g, double* w_const) const override;
    virtual void GetODEJacobian(unsigned int latticepoint, const SpinorFunction& fg, double** jacobian, double* dwdr) const override;

public:
    virtual SpinorFunction ApplyTo(const SpinorFunction& a) const override;

protected:
    virtual SpinorFunction CalculateExtraNonlocal(const SpinorFunction& s, bool include_derivative) const;

protected:
    std::map<int, pSigmaPotential> sigmas;   //!< Map kappa to Sigma
    bool use_fg, use_gg;
    double lambda;
};

typedef boost::shared_ptr<BruecknerDecorator> pBruecknerDecorator;
typedef boost::shared_ptr<const BruecknerDecorator> pBruecknerDecoratorConst;

#endif