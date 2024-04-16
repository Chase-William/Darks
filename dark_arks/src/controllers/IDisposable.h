#ifndef DARKS_CONTROLLERS_IDISPOSABLE_H_
#define DARKS_CONTROLLERS_IDISPOSABLE_H_

/// <summary>
/// A type that contains unmanaged resources that must explicity be cleaned up. For example, implement so that registered hooks into the operating system can be freed before de-allocation.
/// </summary>
class IDisposable {
public:
	void virtual Dispose() = 0;
};

#endif