import 'package:flutter/material.dart';
import 'package:snoozeless/devicesList/devices_list.dart';
import 'package:snoozeless/services/auth.dart';
import 'package:snoozeless/login/login.dart';

class HomeScreen extends StatelessWidget {
  const HomeScreen({Key? key}) : super(key: key);

  @override
  Widget build(BuildContext context) {
    return StreamBuilder(
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
          return const DevicesList();
        } else {
          return const LoginScreen();
        }
      },
    );
  }
}
