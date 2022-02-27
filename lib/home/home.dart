// Flutter packages
import 'package:flutter/material.dart';

// Custom packages
import 'package:snoozeless/services/auth.dart';
import 'package:snoozeless/devices_screen/devices_screen.dart';
import 'package:snoozeless/login/login.dart';

class HomeScreen extends StatelessWidget {
  const HomeScreen({Key? key}) : super(key: key);

  @override
  Widget build(BuildContext context) {
    return StreamBuilder(
      // TODO: Beautify loading and error states.
      stream: AuthService().userStream,
      builder: (context, snapshot) {
        if (snapshot.connectionState == ConnectionState.waiting) {
          return const Center(
            child: Text('Loading'),
          );
        } else if (snapshot.hasError) {
          return const Center(
            child: Text('Error in home.dart'),
          );
        } else if (snapshot.hasData) {
          return const DevicesScreen();
        } else {
          return const LoginScreen();
        }
      },
    );
  }
}
