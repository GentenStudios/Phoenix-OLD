// Copyright 2019 Genten Studios
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice,
// this list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright notice,
// this list of conditions and the following disclaimer in the documentation
// and/or other materials provided with the distribution.
//
// 3. Neither the name of the copyright holder nor the names of its contributors
// may be used to endorse or promote products derived from this software without
// specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.

/***
 * @file ImGuiHelpers.hpp
 * @describe
 *   defines objects that implement alternative ImGui interfaces for easier
 *   UI implementation.
 */

#pragma once

#include <imgui.h>
#include <imgui_internal.h>

#include <iostream>
#include <sstream>
#include <string>
#include <vector>

// NOTE: consider transfering to an ANSI.h for submodule context, this code
//   could possibly be used with an alternative GUI API as well and could
//   benefit from being standalone.
#define ANSI_RED IM_COL32(255, 0, 0, 255)


// NOTE:
//   Directly adding content into the ImGui namespace because as discussed
//   in github: It's far easier to remember and since all of these features
//   extend ImGui it only makes sense that they share the same namespace.
namespace ImGui
{
	/**
	 * @class BaseImWindow
	 * @brief An ancestor / base class for Object Oriented ImGui Windows.
	 *
	 * @detailed
	 *   This ImGui helper class is meant to be a base used to build Object
	 *   Oriented ImGui graphical elements. This is particularly useful when
	 *   trying to create alternatively styleable UI elements. All of the
	 *   classes' functions as per ImGui standard are defined inline by
	 *   default for performance. Initially this was actually made as a
	 *   workaround for ImGui's styling limitations given it's architecture:
	 *   ImGui can only use inline styling.
	 *
	 *   @code
	 *   //This is what I mean; this is the only way to restyle / recolor
	 *   //elements in ImGui outside modifying the global style.
	 *   ImGui::PushStyleColor(ImGuiCol_XXXX, IM_COL32(r,g,b,a));
	 *   //draw a widget or window...
	 *   ImGui::PopStyleColor( none or num of color edits to undo );
	 *   @endcode
	 *
	 *   Although there are other uses for this outside styling such as
	 *   event processing.
	 *
	 *   @note
	 *   This isn't absolutely necessary for styling, but I find separating
	 *   the style logic from the operational logic good for code readability.
	 *
	 *   ###Usage Example:
	 *   Below is an example using this ancestor in practice implemening
	 *   a restylable widget and making a recolor. This has the same effect
	 *   as writing two different functions thanks to inlining, but more of
	 *   the code gets reused as a result. And because there are no method
	 *   abstractions trying to handle the colors, there's no loss in
	 *   performance; with the exception of the virtual function's runtime
	 *   type checker to direct the draw methods to their correct inherited
	 *   functions, but this is negligible in comparison.
	 *
	 *   @note
	 *   The above statement regarding inline optimizations may not be
	 *   correct with all compilers because it's up to the compiler how
	 *   inlining is handled. Thus the performance gains while safe to
	 *   assume true in most scenarios, will not be true in all.
	 *
	 *   @code
	 *   // Basic def for the Object Oriented window we'll be augmenting.
	 *   class ComplexWidget : public BaseImWindow {
	 *   protected:
	 *     const char* firstChildName;
	 *     const char* secondChildName;
	 *     inline void firstChild(ImGuiWindowFlags flags){
	 *       std::string name = std::string(rootWindowName)
	 *                                      .append("_Output");
	 *       ImGui::BeginChild(name.c_str(), flags);
	 *       ImGui::TextWrapped("%s", "first child");
	 *       ImGui::EndChild();
	 *     };
	 *     inline void secondChild(ImGuiWindowFlags flags){
	 *       std::string name = std::string(rootWindowName)
	 *                                      .append("_Output");
	 *       auto ursa = std::string("A uniquely !red stuff and major");
	 *       ImGui::BeginChild(name.c_str(), flags);
	 *       ImGui::TextWrapped("%s", ursa.c_str());
	 *       ImGui::EndChild();
	 *     };
	 *   public:
	 *     using BaseImWindow::BaseImWindow;
	 *
	 *     virtual inline void drawEx(bool* p_open, ImGuiWindowFlags flags){
	 *       begin(p_open, flags);
	 *       firstChild(ImGuiWindowFlags_None);
	 *       secondChild(ImGuiWindowFlags_None);
	 *       end();
	 *     }
	 *   };
	 *
	 *   // Our custom style impelementation.
	 *   class RecolorCW : public ComplexWidget {
	 *   public:
	 *     using BaseImWindow::BaseImWindow;
	 *
	 *     final inline void drawEx(bool* p_open, ImGuiWindowFlags flags){
	 *       begin(p_open, flags);
	 *       firstChild(ImGuiWindowFlags_None);
	 *       PushStyleColor(ImGuiCol_Text, IM_COL32(255,0,0,255));
	 *       secondChild(ImGuiWindowFlags_None);
	 *       PopStyleColor();
	 *       end();
	 *     }
	 *   };
	 *
	 *   ComplexWidget cw("Complexitron");
	 *   RecolorCW rcw("Chromotron");
	 *   int main(){
	 *     //Initialize ImGui, Setup frame loop...
	 *     rcw.draw();
	 *     cw.draw();
	 *     //...
	 *   }
	 *   @endcode
	 */
	class BaseImWindow
	{


	/// @protectedsection
	protected:
		/// @brief
		/// Stores the root window's name when the class is initialized
		/// for use with the call to begin and for extension by child windows.
		const char* rootWindowName;

		/// @param[in] name The root window name when drawn.
		/// @brief Initializes the root window name variable.
		explicit BaseImWindow(const char* name);

	/// @publicsection
	public:
		/**
		 * @param[in,out] p_open
		 *   A boolean pointer that when specified communicates whether
		 *   the root window is in the closed or open state. When NULL
		 *   no window close button is rendered and the window may not be
		 *   closed by ImGui.
		 *
		 * @param[in] flags The ImGuiWindowFlags to be set on the window.
		 *
		 * @brief
		 *   This method serves as a wrapper for the `ImGui::Begin();` function
		 *   directly providing `rootWindowName` though the class as opposed
		 *   to the user passing it for sake of consistency and inheritance
		 *   simplification.
		 *
		 * @note
		 *   This method is intended only to be called within `drawEx`
		 *   to start the root window definition for the graphical element.
		 */
		void begin(bool* p_open, ImGuiWindowFlags flags);

		/**
		 * @brief
		 *   Denote: complementary to the use case per `BaseImWindow::begin`.
		 *   Acts as if calling `ImGui::End();`.
		 *
		 * @note
		 *   This method is intended only to be called within `drawEx`
		 *   to end the root window definition for the graphical element.
		 */
		void end();


		// NOTE: define overrides in base class for ease of implementation
		// down the road.
		//       WARNING: This will not work unless the drawEx funcion is
		//       virtual for runtime
		//                inheritance checking purposes. Otherwise it will
		//                always point to this class' unused version.
		//
		// TODO: vertical shrink with templates
		//
		/**
		 *  @param[in,out] p_open Denote: same as `BaseImWindow::begin`.
		 *  @param[in] flags Denote: same as `BaseImWindow::begin`.
		 *
		 *  @brief
		 *    The public rendering interface for subclasses.
		 *
		 *  @detailed
		 *    Provides an interface for accessing drawEx that doesn't require
		 *    the override cases be redefined in later classes. Be aware that
		 *    if no drawEx funcion is defined in an inheriting class, this
		 *    function will not work.
		 */
		void draw(bool* p_open, ImGuiWindowFlags flags);
		/// @overload
		void draw(bool* p_open);
		/// @overload
		void draw();

		/**
		 *  @param[in,out] p_open
		 *    Denote: Same use as BaseImWindow::begin`.
		 *    This argument is maintained to comply with the ImGui
		 *    standard conventions and must be passed to `begin` by
		 *    the implementing class.
		 *  @param[in] flags
		 *    Denote: Same use as BaseImWindow::begin`.
		 *    This argument is maintained to comply with the ImGui
		 *    standard conventions and must be passed to `begin` by
		 *    the implementing class.
		 *
		 * @brief
		 *   The public draw interface. This handles the actual rendering
		 *   of the drawn class. This function must call `begin`, and `end`
		 *   at the very least.
		 *
		 * @detailed
		 *   This function implements the inherited classes' rendering. It's
		 *   responsible for mapping the base input parameters `p_open` and
		 *   `flags` to the `begin` function for standards compliance and
		 *   should be subclassed to specify non-standard styling. This is
		 *   not intended to be called directly, but instead it is called
		 *   by the inline `draw` functions to abstract away the need to
		 *   redefine the function overrides in inherited classes.
		 *
		 *   ###Usage Example:
		 *   Overriding the `drawEx` function to implement a widget:
		 *
		 *   @code
		 *   // Our custom style impelementation.
		 *   class WinDoesThing : public BaseImWindow {
		 *   private:
		 *     inline void thingy(){
		 *       ImGui::Text("Does a thingy!");
		 *       //...
	 	 *     }
		 *   public:
		 *     //...
		 *     inline void drawEx(bool* p_open, ImGuiWindowFlags flags){
		 *       begin(p_open, flags);
		 *       thingy();
		 *       end();
		 *     }
		 *   };
		 *   @endcode
		 */
		virtual void drawEx(bool* p_open, ImGuiWindowFlags flags);
	};

	/**
	 * @typedef
	 * @brief The callback function pointer format for all terminal callbacks.
	 * @detailed
	 *   Specifically, this is defined as an ease-of-use feature for definitions
	 *   pertaining to the function callbacks within the `BasicTerminal` class.
	 */
	typedef void (*TerminalCallback)(const char* content, std::ostringstream &cout);

	// TODO: More comprehensive example here, I'm too tired to write it atm.
	/**
	 * @class BasicTerminal
	 * @brief An Object Oriented ImGui terminal widget
	 *
	 * @detailed
	 *   The BasicTerminal class is designed to provide a TUI widget though
	 *   ImGui for any kind of display object that requires a single input
	 *   text box, and a single output text box. Such could be a chat, or
	 *   a foux terminal / command line hud element or even a debug terminal.
	 *
	 *   ###Usage Example:
	 *
	 *   @code
	 *   BasicTerminal term("Terminal", 5, "Awaiting events:");
	 *   //...
	 *   // In the render loop:
	 *   term.draw();
	 */
	class BasicTerminal : public BaseImWindow
	{
	/// @privatesection
	private:
		// NOTE:
		//   Because we don't currently have a C++ standard facility for
		//   piping output into a statically allocated string or string_view.
		//   May want to investigate a change here if we upgrade to C++20
		//   Could aid in spliting GPU logic from CPU logic. In case no
		//   data is pushed in one frame, we can avoid senseless resizes.
		//   When this is eventually made thread-safe and the renderer is set
		//   apart from the game logic, should allow for more than one
		//   buffer push to happen before the end of a frame.
		//   Also, we should investigate deriving a custom std::stringbuf
		//   to get access to the underlying string in our std::ostringstream
		//   instead of copying the damn thing and emptying it for every flush
		//   operation. It may also remove the necessity of the flush call
		//   period.
		//
		/// @brief
		///   Copies the contents of the input `std::ostringstream cout` to
		///   the `std::string cache` display stream. Truncates the input
		///   when too large, and manages the contents of our display stream
		///   to keep it within the desired size.
		void flush();

		/// @brief Stores the target output display cache size in bytes.
		int m_targetOutputSize;

		/// @brief Stores the text for our display between updates.
		std::string m_outputBuffer;

		/// @brief The input content from our input box.
		std::string m_inputBuffer;

		/// @brief All the callback function pointers registered with this class.
		std::vector<TerminalCallback> m_callbackRegistry;

		/// @brief The output window name.
		const char* m_outputWindowName;

	///@protectedsection
	protected:
		// TODO:
		//   Maybe use a damn singleton at this point because I'm so
		//   monumentally frustrated with Codacy. Die in a fire you
		//   monstrosity operating under Sonos' name. why I ought ta'
		//   smack ya up one real good! Humbug!
		//
		/// @brief The required flags for this window.
		static const ImGuiWindowFlags defaultFlags =
		    ImGuiWindowFlags(ImGuiWindowFlags_NoScrollbar |
		                     ImGuiWindowFlags_NoScrollWithMouse);

		/// @brief Draws the terminal input field.
		void drawInputField();

		/// @param[in] flags
		///   Specifies what ImGui flags to send to the output window.
		///
		/// @brief Draws the terminal output field.
		void drawOutputField(ImGuiWindowFlags flags);

	///@publicsection
	public:
		/**
		 * @brief
		 *   The cout for the terminal. Piping into this will result in the
		 *   contents being put on the screen.
		 *
		 * @detailed
		 *   This is basically the same as std::cout, but for the ingame
		 *   terminal. Outputting to this will put whatever you passed on the
		 *   screen.
		 *
		 *   The stream is started in output & append at end mode, so
		 *   as I said before, it works just like std::cout, all characters
		 *   are appended to the end of the stream on write.
		 *
		 *   ###Usage Example:
		 *   For external events not govorned by the callback system, you
		 *   may want to output content directly to the terminal; to do so:
		 *
		 *   @code
		 *   BasicTerminal term("Terminal", 5, "Awaiting events:");
		 *   //...
		 *   term.cout << "Output Some Event";
		 *   //...
		 *   @endcode
		 *
		 *   Should result in the following terminal output:
		 *
		 *   Awaiting events:
		 *   Output Some Event
		 */
		std::ostringstream cout;


		/**
		 * @param[in] name Same use as `BaseImWindow`, the root window name.
		 * @param[in] outputKiloBytes The target size of the output cache.
		 * @param[in] initialContents Denote: the initial terminal output text.
		 * @brief
		 *   Initializes the terminal output stream and gets things ready for
		 *   the subsequent draw calls. Initial contents is directly copied
		 *   into the output cache, name is uesd to construct outputWindowName,
		 *   and initialContents is saved for later recall by the flush method.
		 */
		BasicTerminal(const char* name, int outputKiloBytes,
				            const char* initialContents);
		///@overload
		BasicTerminal(const char* name, int outputKiloBytes);


		///@brief Does nothing for now; this is a todo feature.
		~BasicTerminal();

		/**
		 * @param[in] callback A pointer to the desired callback function.
		 * @brief
		 *   Registers callback functions with the terminal. When an input
		 *   event happens, all callbacks will be sent the input string
		 *   in the order said callbacks were registered.
		 *
		 * @detailed
		 *   ###Usage Example:
		 *
		 *   @code
		 *   BasicTerminal term("Terminal", 10, "Some basic output text");
		 *   void cb(const char* input, std::ostringstream &cout){
		 *     cout << input << ", neato!";
		 *   }
		 *   term.registerCallback(&cb);
		 *
		 *   //Typing in this at the terminal:
		 *   //// > stuff and things ////
		 *   // Will yield:
		 *   //// stuff and things, neato! ////
		 *   @endcode
		 */
		void registerCallback(TerminalCallback callback);

		virtual void drawEx(bool* p_open, ImGuiWindowFlags flags);
	};
};

#include "ImGuiHelpers.inl"
