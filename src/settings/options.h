#pragma once
#include <string>

#include <settings/options_type.h>
namespace peachnx::settings {

    class MetaOption {
    public:
        explicit MetaOption(const std::string& name) : label(name) {

        }
        virtual ~MetaOption() = default;

        virtual OptionType GetOptionType() const = 0;
        virtual std::string ToString() const = 0;

    private:
        std::string label;
    };

    template <typename T>
    class Option : public MetaOption {
    public:
        explicit Option(const std::string& name, const T& defaultValue) :
            MetaOption(name), value(defaultValue) {}
        ~Option() override = default;

        OptionType GetOptionType() const override {
            if constexpr (std::is_same_v<T, std::string>)
                return OptionType::String;

            return {};
        }
        std::string ToString() const override {
            if constexpr (std::is_same_v<T, std::string>)
                return value;
            return {};
        }
        auto IsGlobal() const {
            return useGlobal;
        }

        virtual const T& Get() const {
            return value;
        }
    protected:
        T value;
    private:
        bool useGlobal{true};
    };

    template <typename T>
    class MaskableOption final : public Option<T> {
    public:
        explicit MaskableOption(const std::string& name, const T& defaultValue) : Option<T>(name, defaultValue) {}
        ~MaskableOption() override = default;

        const T& Get() const override {
            if (Option<T>::IsGlobal())
                return this->self;
            return local;
        }

        T local;
    };
}