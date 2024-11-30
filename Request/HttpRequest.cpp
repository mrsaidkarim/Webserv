/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zelabbas <zelabbas@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/30 16:00:33 by zelabbas          #+#    #+#             */
/*   Updated: 2024/11/30 18:18:33 by zelabbas         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpRequest.hpp"

HttpRequest::HttpRequest(const string& _request) {
	cout << "HttpRequest constructer\n";
}

HttpRequest::~HttpRequest() {
	cout << "HttpRequest destructer\n";
}

void	HttpRequest::set_status_code(const string& _status) {
	this->statusCode = _status;
}

bool HttpRequest::set_method(const string &method) {
	if (method != "GET" && method != "POST" && method != "DELETE")
		return (this->statusCode = "404" , false);
	else
}