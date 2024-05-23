// Flutter packages
import 'package:flutter/material.dart';

// Custom packages
import 'package:snoozeless/theme.dart';

class EditDeviceBottomButton extends StatelessWidget {
  final Color color;
  final IconData icon;
  final String text;
  final Function method;

  const EditDeviceBottomButton({
    Key? key,
    required this.color,
    required this.icon,
    required this.text,
    required this.method,
  }) : super(key: key);

  @override
  Widget build(BuildContext context) {
    return Flexible(
      fit: FlexFit.tight,
      child: SizedBox(
        height: 50,
        child: OutlinedButton.icon(
          icon: Icon(icon),
          label: Text(text),
          style: OutlinedButton.styleFrom(
            primary: color,
            backgroundColor: appTheme.canvasColor,
            side: BorderSide(
              width: 2.0,
              color: color,
            ),
            shape: RoundedRectangleBorder(
              borderRadius: BorderRadius.circular(5.0),
            ),
          ),
          onPressed: () => method(),
        ),
      ),
    );
  }
}
